#include "Node.h"

namespace cmpn405_datalinklayer
{
    Define_Module(Node);

    uint8_t Node::CRC(const std::string &payload, const uint8_t generator)
    {
        uint8_t rem = 0;

        for (const uint8_t &byte : payload)
        {
            rem ^= byte;
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
        inFile = std::ifstream("../input/" + fileName);
    }

    void Node::initialize() {}

    void Node::handleMessage(cMessage *msg)
    {
        std::string inputPort = msg->getArrivalGate()->getName();
        if (inputPort == "initPort")
            return openFile(msg->getName());

        // send/receive logic

        cancelAndDelete(msg);
    }
}
