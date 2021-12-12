package cmpn405_datalinklayer;

simple Node
{
    parameters:
        int Delay @unit(s) = default(3s);
    gates:
        inout pairPort;
        input initPort;
}
