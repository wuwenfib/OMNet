#ifndef __RECEIVER_H
#define __RECEIVER_H
#include <omnetpp.h>
#include "DataPacket_m.h"
using namespace omnetpp;

class Receiver : public cSimpleModule
{
  protected:
    virtual void initialize() override {}
    virtual void handleMessage(cMessage *msg) override;
};
#endif
