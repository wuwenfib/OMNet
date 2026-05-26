#ifndef __SINK_H
#define __SINK_H
#include <omnetpp.h>
#include <string>
#include "DataPacket_m.h"
using namespace omnetpp;

class Sink : public cSimpleModule
{
  protected:
    std::string label;
    int       received;
    double    totalDelay, maxDelay;
    simtime_t startTime;

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};
#endif
