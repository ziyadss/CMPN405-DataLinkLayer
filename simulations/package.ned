// designate the NED package of this folder
package cmpn405_datalinklayer.simulations;

@license(LGPL);


import ned.IdealChannel;
import cmpn405_datalinklayer.Coordinator;
import cmpn405_datalinklayer.Node;


network DataLinkLayer
{
    parameters:
        int pairs = default(3);
    submodules:
        coordinator: Coordinator;
        nodes[2*pairs]: Node;
        
    connections:
        for i=0..sizeof(nodes)-1
        {
            coordinator.nodeInitPort++ --> IdealChannel --> nodes[i].initPort; 
        }
        
        for i=0..pairs-1
        {
            nodes[2*i].pairPort <--> IdealChannel <--> nodes[2*i+1].pairPort;
        }
        
}