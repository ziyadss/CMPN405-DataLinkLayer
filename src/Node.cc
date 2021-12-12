#include "Node.h"

namespace cmpn405_datalinklayer
{
    Define_Module(Node);

    std::string Node::Framing(std::string msg)
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

    std::string Node::DeFraming(std::string msg)
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

    uint8_t Node::CRC(const std::string payload, const uint8_t crcByte = 0, const uint8_t generator = 0b10011001)
    {
        uint8_t rem = 0;

        for (const uint8_t &byte : payload)
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

    std::vector<std::bitset<8>> Node::toBits(const std::string &payload)
    {
        std::vector<std::bitset<8>> bits(payload.size());

        for (int i = 0; i < payload.size(); i++)
            bits[i] = std::bitset<8>(payload[i]);

        return bits;
    }

    std::string Node::toBytes(const std::vector<std::bitset<8>> &bits)
    {
        std::string bytes(bits.size(), ' ');

        for (int i = 0; i < bits.size(); i++)
            bytes[i] = bits[i].to_ulong();

        return bytes;
    }

    void Node::openFile(const std::string &fileName)
    {
        std::ifstream inFile("../input/" + fileName);

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

    void Node::initialize() {}
    void Node::sendMessage(bool ack = true, unsigned int piggyback_id = 0)
    {
        if (sendQueue.empty())
            return;

        std::string errors = sendQueue.front().first;
        std::string payload = Framing(sendQueue.front().second);
        std::cout << '"' << payload << '"';
        Frame_Base *fmsg = new Frame_Base(
            {message_id++, simTime().dbl()},
            payload.c_str(),
            CRC(payload),
            ack,
            piggyback_id);

        sendQueue.pop();

        send(fmsg, "pairPort$o");
    }

    void Node::receiveMessage(Frame_Base *fmsg)
    {
        uint8_t crcByte = CRC(fmsg->getPayload(), fmsg->getTrailer());
        std::string message = DeFraming(fmsg->getPayload());

        EV << "I am node #" << getIndex() << '\n';
        EV << "Got " << (fmsg->getAck() ? "ACK" : "NACK") << " on message_id " << fmsg->getPiggyback_id() << '\n';
        EV << "Got message #" << fmsg->getHeader().first << " at time " << fmsg->getHeader().second << ": " << message << " -- with CRC: " << std::bitset<8>(crcByte) << '\n';

        sendMessage(!crcByte, fmsg->getHeader().first);
    }

    void Node::handleMessage(cMessage *msg)
    {
        std::string inputPort = msg->getArrivalGate()->getName();
        if (inputPort == "initPort")
        {
            openFile(msg->getName());
            if (msg->getKind())
                sendMessage();
            return;
        }

        Frame_Base *fmsg = check_and_cast<Frame_Base *>(msg);

        receiveMessage(fmsg);

        // send/receive logic
        // CRC("Hello", 0b1001);

        cancelAndDelete(msg);
    }

}
