// designate the NED package of this folder
package cmpn405_datalinklayer.simulations;

@license(LGPL);


import ned.IdealChannel;
import cmpn405_datalinklayer.Coordinator;
import cmpn405_datalinklayer.Node;


network DataLinkLayer
{
    parameters:
        int Pairs = default(3);
        int WindowSize = default(1);
    submodules:
        Coordinator: Coordinator;
        Nodes[2*Pairs]: Node;
        
    connections:
        for i=0..sizeof(Nodes)-1
        {
            Coordinator.nodeInitPort++ --> IdealChannel --> Nodes[i].initPort; 
        }
        
        for i=0..Pairs-1
        {
            Nodes[2*i].pairPort <--> IdealChannel <--> Nodes[2*i+1].pairPort;
        }
        
}