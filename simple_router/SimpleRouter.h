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

    // 三个分支对应三个信号 ID，由 registerSignal() 在 initialize() 里填好
    simsignal_t sigDelivered;   // 到达本机：dest == myAddr
    simsignal_t sigForwarded;   // 命中路由表，向下游转发
    simsignal_t sigDropped;     // 路由表无条目，丢弃

    // 教学用：自己记一份，仅为了 finish() 控制台打印汇总。
    // 真正落盘的统计由 @statistic 自动完成，这两个变量不是必须的。
    long deliveredCount = 0;
    long forwardedCount = 0;
    long droppedCount   = 0;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;   // 仿真结束时由内核调用一次
};

#endif
