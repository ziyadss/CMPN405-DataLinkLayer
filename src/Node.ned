package cmpn405_datalinklayer;

simple Node
{
    parameters:
        double Timeout @unit(s) = default(5s);
        double Delay @unit(s) = default(3s);
    gates:
        inout pairPort;
        input initPort;
}
