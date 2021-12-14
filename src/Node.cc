#include "Node.h"

namespace cmpn405_datalinklayer
{
    Define_Module(Node);

    std::string Node::Framing(const std::string &msg)
    {
        std::string payload("$");

        for (int i = 0; i < msg.size(); i++)
        {
            if (msg[i] == '$' || msg[i] == '/')
            {
                payload = payload + "/";
            }
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

    void Node::initialize()
    {
        timeout_message = new cMessage("Timeout!");
        // TODO: free this at some point
        //       also prevent timeouts sending after sender done so simulation ends
    }

    void Node::sendMessage(const bool ack = true, const int piggyback_id = -1)
    {
        if (sendQueue.empty())
            return;

        std::string errors = sendQueue.front().first;
        std::string payload = Framing(sendQueue.front().second);
        Frame_Base *fmsg = new Frame_Base(
            {message_to_send++, (unsigned int)simTime().dbl()},
            payload.c_str(),
            CRC(payload),
            ack,
            piggyback_id);

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
            EV << "duplicated msg  " << fmsg->getHeader().message_id << endl;
            auto dupMsg = fmsg->dup();
            sendDelayed(dupMsg, 0.01, "pairPort$o");
        }
        double delay = 0;

        //Delay
        if (errors[2] == '1')
            delay = par("Delay").intValue();

        //Loss
        if (errors[3] == '0')
            sendDelayed(fmsg, delay, "pairPort$o");
        else
        {
            cancelAndDelete(fmsg);
            EV << "Lost msg\n";
        }

        sendQueue.pop();
    }

    void Node::receiveMessage(Frame_Base *fmsg)
    {
        cancelEvent(timeout_message);

        const Header header = fmsg->getHeader();

        if (header.message_id == -1)
        {
            EV << "I am node #" << getIndex() << '\n';
            EV << "Got " << (fmsg->getAck() ? "ACK" : "NACK") << " on message_id " << fmsg->getPiggyback_id() << '\n';
            cancelAndDelete(fmsg);
            return sendMessage();
        }

        const uint8_t crcByte = CRC(fmsg->getPayload(), fmsg->getTrailer());
        const std::string message = DeFraming(fmsg->getPayload());

        EV << "I am node #" << getIndex() << '\n';
        // EV << "Got " << (fmsg->getAck() ? "ACK" : "NACK") << " on message_id " << fmsg->getPiggyback_id() << '\n';
        EV << "Got message #" << header.message_id << " at time " << header.timestamp << ": " << message << " -- with CRC: " << std::bitset<8>(crcByte) << '\n';

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
        // SEND (N)ACK ONLY --END

        // sendMessage(ack, header.message_id);
        scheduleAt(simTime() + par("Timeout").intValue(), timeout_message);
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
            return scheduleAt(simTime() + par("Timeout").intValue(), timeout_message);
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
