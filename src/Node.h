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
    void openFile(const std::string &fileName);

    static std::string Framing(const std::string &msg);
    static std::string DeFraming(const std::string &msg);
    static uint8_t CRC(const std::string &payload, const uint8_t crcByte, const uint8_t generator);


    void sendMessage(const bool ack, const int piggyback_id);
    void receiveMessage(Frame_Base *fmsg);

    void openFile(const std::string &fileName);
    void writetoFile(int type,bool ack,int ackNum);
    void calcResults(double totalTime);


    int message_to_send = 0;
    int message_to_receive = 0;

    std::queue<std::pair<std::string, std::string>> sendQueue;
    // std::vector<std::pair<int, std::string>> sendWindow;
    // std::vector<std::string> > receiveWindow;


    cMessage *timeout_message;

    int transNum=0;
    double correctNum=0;


  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
  };
}
