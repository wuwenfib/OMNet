#ifndef __TRAFFICGEN_H
#define __TRAFFICGEN_H
#include <omnetpp.h>
#include "DataPacket_m.h"
using namespace omnetpp;

class TrafficGen : public cSimpleModule
{
  protected:
    double onTime, offTime;
    int    sendRate, pktSize;
    int    seqNum, totalSent;
    cMessage *onMsg, *offMsg, *tickMsg;

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual ~TrafficGen();
};
#endif
