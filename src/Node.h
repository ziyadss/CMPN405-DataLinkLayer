#pragma once

#include <omnetpp.h>
#include <algorithm>
#include <bitset>
#include <fstream>
#include <queue>
#include <stack>
#include "Frame_m.h"

using namespace omnetpp;

namespace cmpn405_datalinklayer
{
  struct message_t
  {
    int id;
    std::string error;
    std::string payload;
    cMessage *timeout_message;
  };

  class Node : public cSimpleModule
  {
    void openFile(const std::string &fileName);

    static std::string Framing(const std::string &msg);
    static std::string DeFraming(const std::string &msg);
    static uint8_t CRC(const std::string &payload, const uint8_t generator);

    void sendMessage(const message_t &message, const bool ack, const int piggyback_id);
    void sender(const bool ack, const int piggyback_id);
    void receiver(Frame_Base *fmsg);

    void writeToFile(int type, bool ack, int ackNum, int msg_id);
    void calcResults(double totalTime);

    std::queue<message_t> sendQueue;
    std::stack<std::string> receiveStack;

    std::vector<message_t> sendWindow;
    std::set<std::pair<int, std::string>> receiveBuffer;

    int windowSize = getParentModule()->par("WindowSize").intValue();
    int message_to_receive = 0;

    int transNum = 0;
    double correctNum = 0;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
  };
}
