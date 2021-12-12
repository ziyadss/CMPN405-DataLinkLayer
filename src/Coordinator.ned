package cmpn405_datalinklayer;

simple Coordinator
{
    parameters:
        string InputFolder = default("original");
    gates:
        output nodeInitPort[];
}
