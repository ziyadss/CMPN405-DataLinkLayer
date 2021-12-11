#pragma once

#include <omnetpp.h>
#include <bitset>
#include <fstream>

using namespace omnetpp;

namespace cmpn405_datalinklayer
{
  class Node : public cSimpleModule
  {
    static uint8_t CRC(const std::string &payload, const uint8_t generator);
    static std::vector<std::bitset<8>> toBits(const std::string &payload);
    static std::string toBytes(const std::vector<std::bitset<8>> &bits);

    void openFile(const std::string &fileName);

    std::ifstream inFile;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
  };
}