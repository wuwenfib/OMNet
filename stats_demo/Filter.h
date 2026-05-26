#ifndef FILTER_H_
#define FILTER_H_

#include <omnetpp.h>
using namespace omnetpp;

class Filter : public cSimpleModule {
  private:
    double dropProb;
    simsignal_t sigForwarded;   // registerSignal() 返回的整数 ID，emit 时用
    simsignal_t sigDropped;

    // 自己维护一份计数，纯粹是为了 finish() 在控制台打印汇总。
    // @statistic 已经把数据写进 .sca 了，这两个变量只是教学可视化。
    long fwdCount  = 0;
    long dropCount = 0;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;   // 仿真结束时由内核回调一次
};

#endif
