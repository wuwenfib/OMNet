#ifndef __LEAKYBUCKET_H
#define __LEAKYBUCKET_H
#include <omnetpp.h>
#include <queue>
#include "DataPacket_m.h"
using namespace omnetpp;

class LeakyBucket : public cSimpleModule
{
  protected:
    int    bucketSize;
    double drainRate;
    int    dropped, forwarded;
    std::queue<DataPacket*> pktQueue;
    cMessage *drainTimer;

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual ~LeakyBucket();
};
#endif
