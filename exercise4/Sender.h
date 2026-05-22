#ifndef __SENDER_H
#define __SENDER_H
#include <omnetpp.h>
#include "DataPacket_m.h"
using namespace omnetpp;

class Sender : public cSimpleModule
{
  protected:
    int    srcAddr;
    double interval;
    int    counter;
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};
#endif
