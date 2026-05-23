#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <omnetpp.h>
#include "Job_m.h"

using namespace omnetpp;

class Generator : public cSimpleModule {
  private:
    double interArrivalMean;
    long jobCount;
    cMessage *genTimer;     // 自消息：触发"产生下一个 Job"

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

  public:
    virtual ~Generator();
};

#endif
