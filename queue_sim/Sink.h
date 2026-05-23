#ifndef SINK_H_
#define SINK_H_

#include <omnetpp.h>
#include "Job_m.h"

using namespace omnetpp;

class Sink : public cSimpleModule {
  private:
    long   servedCount;       // 服务完成总数
    double sojournTimeSum;    // 停留时间总和（生成 -> 离开系统）

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

#endif
