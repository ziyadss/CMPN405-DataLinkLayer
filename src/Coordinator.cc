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

#include "Coordinator.h"
#include <fstream>

namespace cmpn405_datalinklayer {

Define_Module(Coordinator);

void Coordinator::initialize()
{
    // TODO - Generated method body
//    std::ifstream inFile("../input/coordinator.txt");
//    std::string start_or_nodeID;
//    while (!inFile.eof())
//    {
//        std::string fileName;
//        int nodeID = std::stoi(start_or_nodeID);
//        inFile >> fileName >> start_or_nodeID;
//        if (start_or_nodeID == "start")
//        {
//            int startTime;
//            inFile >> startTime;
//            // Start 'nodeID' with file 'fileName' at time 'startTime'
//            inFile >> start_or_nodeID;
//        }
//        else
//        {
//            // 'std::stoi(start_or_nodeID)' does not send first
//        }
//    }
}

void Coordinator::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

} //namespace
