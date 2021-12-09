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

#ifndef __CMPN405_DATALINKLAYER_NODE_H_
#define __CMPN405_DATALINKLAYER_NODE_H_

#include <omnetpp.h>
#include <bitset>

using namespace omnetpp;

namespace cmpn405_datalinklayer {

/**
 * TODO - Generated class
 */
class Node : public cSimpleModule
{
    uint8_t CRC(const std::string &payload, const uint8_t generator);
    std::vector<std::bitset<8>> toBits(const std::string &payload);
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

} //namespace

#endif
