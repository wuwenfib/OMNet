#ifndef SENDER_H_
#define SENDER_H_

#include <omnetpp.h>
#include "AppPacket_m.h"

using namespace omnetpp;

class Sender : public cSimpleModule {
  private:
    int srcAddr;
    double interval;
    int sendIndex;          // 已发包计数，用作目的地址列表的下标

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
