#ifndef HOST_H_
#define HOST_H_

#include <omnetpp.h>
#include <vector>
#include <string>
#include "AppPacket_m.h"

using namespace omnetpp;

class Host : public cSimpleModule {
  private:
    // ---- NED 参数 ----
    int    myAddr;
    double interArrivalMean;
    int    initialTtl;

    // ---- 目的地列表 + 流水号 ----
    std::vector<int> destinations;
    long nextSerial;

    // ---- 自消息：发送定时器 ----
    cMessage *sendTimer;

    // ---- 信号 ----
    simsignal_t sigGenerated;
    simsignal_t sigDelivered;

    void parseDestinations(const std::string &spec);

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

  public:
    virtual ~Host();
};

#endif
