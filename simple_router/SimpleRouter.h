#ifndef SIMPLEROUTER_H_
#define SIMPLEROUTER_H_

#include <omnetpp.h>
#include <map>
#include "AppPacket_m.h"

using namespace omnetpp;

class SimpleRouter : public cSimpleModule {
  private:
    int myAddr;
    std::map<int, int> routingTable;  // destAddr -> 输出门索引

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
