#ifndef __ROUTER_H
#define __ROUTER_H
#include <omnetpp.h>
#include "DataPacket_m.h"
using namespace omnetpp;

class Router : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};
#endif
