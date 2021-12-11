#include "Coordinator.h"

namespace cmpn405_datalinklayer
{
    Define_Module(Coordinator);

    void Coordinator::initialize()
    {
        std::ifstream inFile("../input/coordinator.txt");
        std::string start_or_nodeID;
        inFile >> start_or_nodeID;
        while (!inFile.eof())
        {
            std::string fileName;
            int nodeID = std::stoi(start_or_nodeID);
            inFile >> fileName >> start_or_nodeID;
            if (start_or_nodeID == "start")
            {
                int startTime;
                inFile >> startTime;
                // Start 'nodeID' with file 'fileName' at time 'startTime'
                cMessage *msg = new cMessage(fileName.c_str());
                sendDelayed(msg, startTime, "nodeInitPort", nodeID);
                EV << "Node " << nodeID << " with file " << fileName << " at time " << startTime << std::endl;
                inFile >> start_or_nodeID;
            }
            else
            {
                // 'nodeID' does not send first
                cMessage *msg = new cMessage(fileName.c_str());
                send(msg, "nodeInitPort", nodeID);
                EV << "Node " << nodeID << " with file " << fileName << std::endl;
            }
        }
    }

    void Coordinator::handleMessage(cMessage *msg) {}
}