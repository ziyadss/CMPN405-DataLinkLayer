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
        Frame_Base *fmsg = new Frame_Base(
            {message.id, (unsigned int)simTime().dbl()},
            payload.c_str(),
            CRC(payload),
            ack,
            piggyback_id);

        transNum++;
        int type = 0;
        double delay = 0;

        if (message.error[0] == '1')
        {
            //Modify
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
            writeToFile(type, ack, piggyback_id, message.id);
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

        writeToFile(type, ack, piggyback_id, message.id);
        scheduleAt(simTime() + par("Timeout").doubleValue(), message.timeout_message);
    }

    void Node::sender(const bool ack = true, const int piggyback_id = -1)
    {
        if (sendQueue.empty())
        {
            Frame_Base *fmsg = new Frame_Base(
                {-1, (unsigned int)simTime().dbl()},
                nullptr,
                0,
                ack,
                piggyback_id);
            send(fmsg, "pairPort$o");
            // calcResults(simTime().dbl());
            return;
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
        }
    }

    void Node::receiver(Frame_Base *fmsg)
    {

        // if ACK, remove it from the sendWindow
        // when I get ACK I do
        // cancelEvent(message.timeout_message);
        // delete message.timeout_message;
        // message.timeout_message = nullptr;

        /* 

        fmsg->getHeader().message_id 
            is -1 if no message, number if there is a message

        fmsg->getPiggyback_id()
            is -1 if no (n)ack, number if there is (n)ack
        
        */
        //check if the msg has Ack or not
        EV << "I am node #" << getIndex() << '\n';
        if (const int pb_id = fmsg->getPiggyback_id(); pb_id != -1)
        {
            const bool ack = fmsg->getAck();
            EV << "Got " << (ack ? "ACK" : "NACK") << " on message id " << pb_id << '\n';
            correctNum += ack;

            //advance the sliding window
            while (ack && sendWindow.size() > 0 && sendWindow.front().id <= pb_id)
                sendWindow.erase(sendWindow.begin());
        }

        const Header header = fmsg->getHeader();

        if (header.message_id != -1)
        {
            const std::string crcString = std::string(fmsg->getPayload()) + fmsg->getTrailer();
            const uint8_t crcByte = CRC(crcString);
            const std::string message = DeFraming(fmsg->getPayload());

            EV << "Got message #" << header.message_id << " at time " << header.timestamp << ": " << message << " -- with CRC: " << std::bitset<8>(crcByte) << '\n';

            bool ack = false;
            //if msg is correct, and it's more than the frame expected and it's within the windowSize
            if (!crcByte && header.message_id >= message_to_receive && header.message_id < message_to_receive + windowSize)
            {
                ack = true;
                receiveBuffer.insert({header.message_id, message});
            }
            //else send Nack TODO

            //move msgs from Buffer to Stack when the first msg is received and and when there's anything in the buffer
            while (receiveBuffer.size() > 0 && receiveBuffer.begin()->first == message_to_receive)
            {
                message_to_receive++;
                receiveStack.push(receiveBuffer.begin()->second);
                receiveBuffer.erase(receiveBuffer.begin());
            }

            writeToFile(1, ack, message_to_receive, header.message_id);
            sender(ack, message_to_receive);
        }

        cancelAndDelete(fmsg);
    }

    void Node::writeToFile(int type, bool ack, int ackNum, int msg_id)
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

        // if (type == 0)
        //     result = result + " and content = \"" + sendQueue.front().second + "\"";
        // else if (type == 1)
        //     result = result + " and content = \"" + receiveStack.top() + "\"";

        if (type != 2)
            result = result + " at " + std::to_string(simTime().dbl());

        if (ack && (type == 0 || type == 1))
        {
            // std::string errors = sendQueue.front().first;
            // if (errors[0] == '1')
            //     result += " with modification ";

            result = result + " and piggybacking Ack number " + std::to_string(ackNum);
        }
        else if (type == 0 || type == 1)
            result = result + " and NACK number " + std::to_string(ackNum);

        outFile << result << endl;
    }

    void Node::calcResults(double totalTime)
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

        outFile << "- '''''''''''' " << endl;

        if (getIndex() == 0 || getIndex() == 1)
        {
            outFile << "- node 0 end of input file" << endl;
            outFile << "- node 1 end of input file" << endl;
        }
        else if (getIndex() == 2 || getIndex() == 3)
        {
            outFile << "- node 2 end of input file" << endl;
            outFile << "- node 3 end of input file" << endl;
        }
        else
        {
            outFile << "- node 4 end of input file" << endl;
            outFile << "- node 5 end of input file" << endl;
        }
        outFile << "- total transmission time = " << totalTime << endl;
        outFile << "- total number of transmissions = " << transNum << endl;
        outFile << "- the network throughput = " << correctNum / totalTime << endl;
        outFile.close();
    }

    void Node::initialize()
    {
        // windowSize = getParentModule()->par("WindowSize");
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
