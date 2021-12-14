#include "Node.h"

namespace cmpn405_datalinklayer
{
    Define_Module(Node);
    void Node::openFile(const std::string &fileName)
    {
        std::ifstream inFile(fileName);

        std::string line;
        while (!inFile.eof())
        {
            std::string errorTemp, msgTemp;
            getline(inFile, line);
            errorTemp = line.substr(0, 4);
            msgTemp = line.substr(5, line.length());
            sendQueue.push({errorTemp, msgTemp});
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

    uint8_t Node::CRC(const std::string &payload, const uint8_t crcByte = 0, const uint8_t generator = 0b10011001)
    {
        uint8_t rem = 0;

        for (const uint8_t byte : payload)
        {
            rem ^= byte;
            for (int i = 0; i < 8; i++)
                rem = rem << 1 ^ (rem >> 7) * generator;
        }

        if (crcByte)
        {
            rem ^= crcByte;
            for (int i = 0; i < 8; i++)
                rem = rem << 1 ^ (rem >> 7) * generator;
        }

        return rem;
    }

    void Node::sendMessage(const bool ack = true, const int piggyback_id = -1)
    {
        if (sendQueue.empty())
        {
            Frame_Base *fmsg = new Frame_Base(
                {-1, (unsigned int)simTime().dbl()},
                nullptr,
                0,
                0,
                -1);
            send(fmsg, "pairPort$o");
            calcResults(simTime().dbl());
            return;
        }

        std::string errors = sendQueue.front().first;
        std::string payload = Framing(sendQueue.front().second);
        Frame_Base *fmsg = new Frame_Base(
            {message_to_send++, (unsigned int)simTime().dbl()},
            payload.c_str(),
            CRC(payload),
            ack,
            piggyback_id);

        transNum++;
        int type = 0;
        double delay = 0;

        if (errors[0] == '1')
        {
            //Modify
            int rand = uniform(0, 1) * payload.length();
            EV << "rand is " << std::to_string(rand) << endl;
            payload[rand] = payload[rand] + 5;
            fmsg->setPayload(payload.c_str());
        }
        if (errors[1] == '1')
        {
            //Duplicate
            transNum++;
            EV << "duplicated msg  " << fmsg->getHeader().message_id << endl;
            auto dupMsg = fmsg->dup();
            sendDelayed(dupMsg, 0.01, "pairPort$o");
            writeToFile(type, ack, piggyback_id, message_to_send - 1);
        }

        //Delay
        if (errors[2] == '1')
            delay = par("Delay").doubleValue();

        //Loss
        if (errors[3] == '0')
            sendDelayed(fmsg, delay, "pairPort$o");
        else
        {
            type = 2;
            cancelAndDelete(fmsg);
            EV << "Lost msg\n";
        }

        writeToFile(type, ack, piggyback_id, message_to_send - 1);

        sendQueue.pop();
    }

    void Node::receiveMessage(Frame_Base *fmsg)
    {
        if (timeout_message)
            cancelEvent(timeout_message);

        const Header header = fmsg->getHeader();

        if (header.message_id == -1)
        {
            if (timeout_message)
            {
                delete timeout_message;
                timeout_message = nullptr;
            }

            if (fmsg->getPiggyback_id() == -1)
                return;

            EV << "I am node #" << getIndex() << '\n';
            EV << "Got " << (fmsg->getAck() ? "ACK" : "NACK") << " on message_id " << fmsg->getPiggyback_id() << '\n';
            correctNum += fmsg->getAck();
            cancelAndDelete(fmsg);
            return sendMessage();
        }

        const uint8_t crcByte = CRC(fmsg->getPayload(), fmsg->getTrailer());
        const std::string message = DeFraming(fmsg->getPayload());

        EV << "I am node #" << getIndex() << '\n';
        // EV << "Got " << (fmsg->getAck() ? "ACK" : "NACK") << " on message_id " << fmsg->getPiggyback_id() << '\n';
        EV << "Got message #" << header.message_id << " at time " << header.timestamp << ": " << message << " -- with CRC: " << std::bitset<8>(crcByte) << '\n';

        receiveStack.push(message);

        cancelAndDelete(fmsg);

        // SEND (N)ACK ONLY --START
        const bool ack = !crcByte && header.message_id == message_to_receive;
        message_to_receive += ack;
        fmsg = new Frame_Base(
            {-1, (unsigned int)simTime().dbl()},
            nullptr,
            0,
            ack,
            message_to_receive);

        sendDelayed(fmsg, 0.2, "pairPort$o");
        writeToFile(1, ack, message_to_receive, header.message_id);
        // SEND (N)ACK ONLY --END

        // sendMessage(ack, header.message_id);
        if (timeout_message)
            scheduleAt(simTime() + par("Timeout").doubleValue(), timeout_message);
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

        std::string result = "- node " + std::to_string(getIndex());

        // types 0-send 1-recieve 2-drop 3-timeout
        if (type == 0)
            result += " sends message with id=" + std::to_string(msg_id);
        else if (type == 1)
            result += " received message with id=" + std::to_string(msg_id);
        else if (type == 2)
            result += " drops message with id=" + std::to_string(msg_id);
        else
            result += " timeout for message id=" + std::to_string(msg_id);

        if (type == 0)
            result = result + " and content= " + sendQueue.front().second;
        else if (type == 1)
            result = result + " and content= " + receiveStack.top();

        if (type != 2)
            result = result + " at " + std::to_string(simTime().dbl());

        if (ack && (type == 0 || type == 1))
        {
            std::string errors = sendQueue.front().first;
            if (errors[0] == '1')
                result += " with modification ";

            result = result + "and piggybacking Ack number " + std::to_string(ackNum);
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
        outFile << "- total transmission time= " << totalTime << endl;
        outFile << "- total number of transmissions= " << transNum << endl;
        outFile << "- the network throughput= " << correctNum / totalTime << endl;
        outFile.close();
    }

    void Node::initialize()
    {
        timeout_message = new cMessage("Timeout!");
    }

    void Node::handleMessage(cMessage *msg)
    {
        if (msg->isSelfMessage())
        {
            EV << "Timeout!\n";
            Frame_Base *fmsg = new Frame_Base(
                {-1, (unsigned int)simTime().dbl()},
                nullptr,
                0,
                0,
                message_to_receive);

            send(fmsg, "pairPort$o");
            return scheduleAt(simTime() + par("Timeout").doubleValue(), timeout_message);
        }

        const std::string inputPort = msg->getArrivalGate()->getName();
        if (inputPort == "initPort")
        {
            openFile(msg->getName());
            if (msg->getKind())
                sendMessage();

            return cancelAndDelete(msg);
        }

        Frame_Base *fmsg = check_and_cast<Frame_Base *>(msg);
        receiveMessage(fmsg);
    }
}
