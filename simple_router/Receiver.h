#ifndef RECEIVER_H_
#define RECEIVER_H_

#include <omnetpp.h>
#include "AppPacket_m.h"

using namespace omnetpp;

class Receiver : public cSimpleModule {
  private:
    int myAddr;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
