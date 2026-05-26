#ifndef SOURCE_H_
#define SOURCE_H_

#include <omnetpp.h>
using namespace omnetpp;

class Source : public cSimpleModule {
  private:
    int numPackets;
    int sent;
    double interval;
    cMessage *timer = nullptr;
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
  public:
    ~Source() override { cancelAndDelete(timer); }
};

#endif
