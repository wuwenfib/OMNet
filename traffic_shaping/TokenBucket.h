#ifndef __TOKENBUCKET_H
#define __TOKENBUCKET_H
#include <omnetpp.h>
#include "DataPacket_m.h"
using namespace omnetpp;

class TokenBucket : public cSimpleModule
{
  protected:
    int    bucketSize;
    double tokenRate;
    int    tokens;
    int    dropped, forwarded;
    cMessage *tokenTimer;

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual ~TokenBucket();
};
#endif
