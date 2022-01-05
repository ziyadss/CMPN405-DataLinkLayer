#include "Node.h"

namespace cmpn405_datalinklayer
{
    Define_Module(Node);
    void Node::openFile(const std::string &fileName)
    {
        std::ifstream inFile(fileName);

        std::string line;
        int id = 0;

        while (!inFile.eof())
        {
            getline(inFile, line);
            std::string error = line.substr(0, 4);
            std::string payload = line.substr(5, line.length());
            sendQueue.push({id++, error, payload, nullptr});
        }
    }

    std::string Node::Framing(const std::string &msg)
    {
        std::string payload("$");

        for (int i = 0; i < msg.size(); i++)
        {
            if (msg[i] == '$' || msg[i] == '/')
                payload = payload + "/";

            payload = payload + msg[i];
        }
        payload.append("$");
        return payload;
    }

    std::string Node::DeFraming(const std::string &msg)
    {
        int i = 1;
        std::string payload;
        while (i < msg.size() - 1)
        {
            if (msg[i + 1] == '/')
            {
                payload = payload + msg[i];
                i += 2;
            }
            else
            {
                payload = payload + msg[i];
                i++;
            }
        }
        return payload;
    }

    uint8_t Node::CRC(const std::string &payload, const uint8_t generator = 0b10011001)
    {
        uint8_t rem = 0;

        for (const uint8_t byte : payload)
        {
            rem ^= byte;
            for (int i = 0; i < 8; i++)
                rem = rem << 1 ^ (rem >> 7) * generator;
        }

        return rem;
    }

    void Node::sendMessage(const message_t &message, const bool ack = true, const int piggyback_id = -1)
    {
        std::string payload = Framing(message.payload);
        if (useHamming)
            payload = hamming_encode(payload);
        Frame_Base *fmsg = new Frame_Base(
            {message.id, (unsigned int)simTime().dbl()},
            payload.c_str(),
            CRC(payload),
            ack,
            piggyback_id);

        transNum++;
        int type = 0;
        double delay = 0;
        bool modified = false;
        if (message.error[0] == '1')
        {
            //Modify
            modified = true;
            int rand = uniform(0, payload.length());
            int bit = uniform(0, 8);
            EV << "Modified byte " << rand << ", bit " << bit << endl;

            payload[rand] ^= 1 << bit;
            fmsg->setPayload(payload.c_str());
        }
        if (message.error[1] == '1')
        {
            //Duplicate
            transNum++;
            EV << "Duplicated message " << message.id << std::endl;
            Frame_Base *dupMsg = fmsg->dup();
            sendDelayed(dupMsg, 0.01, "pairPort$o");
            writeToFile(type, ack, piggyback_id, message.id, message.payload, modified);
        }

        //Delay
        if (message.error[2] == '1')
            delay = par("Delay").doubleValue();

        //Loss
        if (message.error[3] == '0')
            sendDelayed(fmsg, delay, "pairPort$o");
        else
        {
            type = 2;
            EV << "Lost message " << message.id << std::endl;
            cancelAndDelete(fmsg);
        }

        writeToFile(type, ack, piggyback_id, message.id, message.payload, modified);
        scheduleAt(simTime() + par("Timeout").doubleValue(), message.timeout_message);
    }

    void Node::sender(const bool ack = true, const int piggyback_id = -1, int last_rcv = 0)
    {
        if (sendQueue.empty())
        {
            if (last_rcv == -1 && sendWindow.empty())
                return; // calcResults(simTime().dbl());

            Frame_Base *fmsg = new Frame_Base(
                {-1, (unsigned int)simTime().dbl()},
                nullptr,
                0,
                ack,
                piggyback_id);
            return send(fmsg, "pairPort$o");
        }

        // Move messages from sendQueue to sendWindow
        while (sendWindow.size() < windowSize && sendQueue.size() > 0)
        {
            auto message = sendQueue.front();
            sendQueue.pop();

            // Prepare timeout timer and push the message to sendWindow
            auto timeout_text = std::to_string(message.id);
            message.timeout_message = new cMessage(timeout_text.c_str());
            sendWindow.push_back(message);

            //send next msg
            sendMessage(message, ack, piggyback_id);
            message.error = "0000"; // to make sure that when we send the Nack we resend the msg with 0 error

            EV << "sendWindow size: " << std::to_string(sendWindow.size()) << std::endl;
        }
    }

    void printMessageVec(std::vector<message_t> messageQueue)
    {
        EV << "Printing message queue" << std::endl;
        for (auto message : messageQueue)
        {
            EV << "Message id: " << message.id << std::endl;
            EV << "Message payload: " << message.payload << std::endl;
        }
    };

    void Node::receiver(Frame_Base *fmsg)
    {

        //check if the msg has Ack or not
        EV << "I am node #" << getIndex() << '\n';
        if (const int pb_id = fmsg->getPiggyback_id(); pb_id != -1)
        {
            const bool ack = fmsg->getAck();
            EV << "Got " << (ack ? "ACK" : "NACK") << " on message id " << pb_id << '\n';
            correctNum += ack;

            //advance the sliding window

            printMessageVec(sendWindow);

            if (!ack)
            {
                if (last_nack == pb_id)
                    return;
                last_nack = pb_id;
                message_t to_send = *std::find_if(sendWindow.begin(), sendWindow.end(), [&](const message_t &message)
                                                  { return message.id == pb_id; });

                cancelEvent(to_send.timeout_message);
                to_send.error = "0000";
                return sendMessage(to_send);
            }
            else
                while (sendWindow.size() > 0 && sendWindow.front().id < pb_id)
                {
                    auto it = sendWindow.begin();
                    cancelEvent(it->timeout_message);
                    delete it->timeout_message;
                    sendWindow.erase(it);

                    EV << "message removed from sendWindow, size now: " << std::to_string(sendWindow.size()) << std::endl;
                }
        }

        const Header header = fmsg->getHeader();
        bool ack = true, no_error = true;
        std::string message = "";
        if (header.message_id != -1)
        {
            message = DeFraming(fmsg->getPayload());

            int error_loc = -1;
            uint8_t crcByte;
            if (useHamming)
            {
                auto error_and_msg = hamming_decode(message);
                message = error_and_msg.second;
                error_loc = error_and_msg.first;
            }
            else
            {
                const std::string crcString = std::string(fmsg->getPayload()) + fmsg->getTrailer();
                crcByte = CRC(crcString);
            }

            EV << "Got message #" << header.message_id << " at time " << header.timestamp << ": " << message;

            if (useHamming)
            {
                if (error_loc == -1)
                    EV << " -- with no error\n";
                else
                    EV << "-- with error located at bit " << std::to_string(error_loc) << " and fixed\n";
            }
            else
                EV << " -- with CRC: " << std::bitset<8>(crcByte) << '\n';

            no_error = useHamming ? error_loc == -1 : !crcByte;
            ack = no_error && header.message_id == message_to_receive;
            bool accepted = no_error && header.message_id >= message_to_receive && header.message_id < message_to_receive + windowSize;
            EV << "no_error: " << std::to_string(no_error) << " header.message_id: " << std::to_string(header.message_id) << " message_to_receive: " << std::to_string(message_to_receive) << " windowSize: " << std::to_string(windowSize) << " ack: " << std::to_string(ack) << std::endl;
            EV << "accepted: " << std::to_string(accepted) << std::endl;
            EV << "ack: " << std::to_string(ack) << std::endl;
            //if msg is correct, and it's more than the frame expected and it's within the windowSize
            if (accepted)
            {
                receiveBuffer.insert({header.message_id, message});
                EV << "Buffered message #" << header.message_id << '\n';
            }

            //else send Nack TODO

            //move msgs from Buffer to Stack when the first msg is received and and when there's anything in the buffer
            while (receiveBuffer.size() > 0 && receiveBuffer.begin()->first == message_to_receive)
            {
                message_to_receive++;
                receiveStack.push(receiveBuffer.begin()->second);
                receiveBuffer.erase(receiveBuffer.begin());
                EV << "Message #" << message_to_receive - 1 << " moved to receiveStack\n";
            }
        }

        writeToFile(1, fmsg->getAck(), fmsg->getPiggyback_id(), header.message_id, message, !no_error);
        sender(ack, message_to_receive, header.message_id);

        cancelAndDelete(fmsg);
    }

    void Node::writeToFile(int type, bool ack, int ackNum, int msg_id, std::string message, bool modified = false)
    {
        std::ofstream outFile;
        std::string filename;
        if (getIndex() == 0 || getIndex() == 1)
            filename = "pair01.txt";
        else if (getIndex() == 2 || getIndex() == 3)
            filename = "pair23.txt";
        else
            filename = "pair45.txt";

        outFile.open(filename, std::fstream::app);
        if (!outFile)
            EV << "Error in opening output file" << endl;

        std::string result = "- Node " + std::to_string(getIndex());

        // types 0-send 1-recieve 2-drop 3-timeout
        if (type == 0)
            result += " sends message with id = ";
        else if (type == 1)
            result += " received message with id = ";
        else if (type == 2)
            result += " drops message with id = ";
        else
            result += " timeout for message id = ";

        result += std::to_string(msg_id);

        if (type == 0 || type == 1)
            result = result + " and content = \"" + message + "\"";

        if (type != 2)
            result = result + " at " + std::to_string(simTime().dbl());

        if (ack && (type == 0 || type == 1))
        {
            if (modified)
                result += " with modification";

            result = result + " and piggybacking ACK number " + std::to_string(ackNum);
        }
        else if (type == 0 || type == 1)
            result = result + " and piggybacking NACK number " + std::to_string(ackNum);

        outFile << result << endl;
    }

    void Node::calcResults(double totalTime)
    {
        std::string filename;
        if (getIndex() == 0 || getIndex() == 1)
            filename = "pair01.txt";
        else if (getIndex() == 2 || getIndex() == 3)
            filename = "pair23.txt";
        else
            filename = "pair45.txt";

        std::ofstream outFile(filename, std::fstream::app);
        if (!outFile)
            EV << "Error in opening output file" << endl;

        outFile << "- '''''''''''' " << endl;
        outFile << "- Node " << getIndex() << " end of input file" << endl;
        outFile << "- Total transmission time = " << totalTime << endl;
        outFile << "- Total number of transmissions = " << transNum << endl;
        outFile << "- The network throughput = " << correctNum / totalTime << endl;
        outFile.close();
    }

    void Node::initialize()
    {
        windowSize = getParentModule()->par("WindowSize").intValue();
        useHamming = getParentModule()->par("UseHamming").boolValue();
    }

    void Node::handleMessage(cMessage *msg)
    {
        if (msg->isSelfMessage())
        {
            EV << "Timeout!\n";
            int message_id = atoi(msg->getName());

            message_t to_send = *std::find_if(sendWindow.begin(), sendWindow.end(), [&](const message_t &message)
                                              { return message.id == message_id; });

            return sendMessage(to_send);
        }

        const std::string inputPort = msg->getArrivalGate()->getName();
        if (inputPort == "initPort")
        {
            openFile(msg->getName());
            if (msg->getKind())
                sender();

            return cancelAndDelete(msg);
        }

        Frame_Base *fmsg = check_and_cast<Frame_Base *>(msg);
        receiver(fmsg);
    }
}
