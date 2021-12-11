#pragma once

#include <omnetpp.h>
#include <fstream>

using namespace omnetpp;

namespace cmpn405_datalinklayer
{
  class Coordinator : public cSimpleModule
  {
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
  };
}