package cmpn405_datalinklayer;

simple Node
{
    parameters:
        int Timeout @unit(s) = default(5s);
        int Delay @unit(s) = default(3s);
    gates:
        inout pairPort;
        input initPort;
}
