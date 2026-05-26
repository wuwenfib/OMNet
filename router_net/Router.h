#ifndef ROUTER_H_
#define ROUTER_H_

#include <omnetpp.h>
#include <map>
#include <string>
#include "AppPacket_m.h"

using namespace omnetpp;

class Router : public cSimpleModule {
  private:
    // ---- NED 参数 ----
    int myAddr;
    int defaultPort;

    // ---- 路由表：destAddr -> portIdx ----
    std::map<int, int> routingTable;

    // ---- 信号句柄（emit 时用） ----
    simsignal_t sigForwarded;
    simsignal_t sigDroppedNoRoute;
    simsignal_t sigTtlExpired;

    // ---- 辅助函数 ----
    void parseRoutingTable(const std::string &spec);

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
