#pragma once

#include <omnetpp.h>
#include <bitset>
#include <fstream>
#include <queue>
#include "Frame_m.h"

using namespace omnetpp;

namespace cmpn405_datalinklayer
{
  class Node : public cSimpleModule
  {
    static uint8_t CRC(const std::string payload, const uint8_t crcByte, const uint8_t generator);
    static std::vector<std::bitset<8>> toBits(const std::string &payload);
    static std::string toBytes(const std::vector<std::bitset<8>> &bits);

    void sendMessage(bool ack, int piggyback_id);
    void receiveMessage(Frame_Base *fmsg);

    void openFile(const std::string &fileName);
    void writetoFile(int type,bool ack,int ackNum);

    static std::string Framing(std::string msg);
    static std::string DeFraming(std::string msg);

    std::queue<std::pair<std::string, std::string>> sendQueue;
    // std::vector<std::pair<int, std::string>> sendQueue;
    // std::vector<std::string> > receiveQueue;

    unsigned int message_id = 0;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
  };
}