//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Node.h"

namespace cmpn405_datalinklayer {

Define_Module(Node);

uint8_t Node::CRC(const std::string &payload, const uint8_t generator)
{
    uint16_t rem = 0;
    for (const char &c : payload)
        rem = ((rem << 8) + c) % generator;

    int genMSB = log2(generator);
    rem = (rem << genMSB) % generator;

    return rem >>= 1;
}

std::vector<std::bitset<8>> Node::toBits(const std::string &payload)
{
    std::vector<std::bitset<8>> bits(payload.size());

    for (int i = 0; i < payload.size(); i++)
        bits[i] = std::bitset<8>(payload[i]);

    return bits;
}

void Node::initialize()
{
    // TODO - Generated method body
}

void Node::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

} //namespace
