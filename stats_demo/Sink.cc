#include <omnetpp.h>
using namespace omnetpp;

class Sink : public cSimpleModule {
  protected:
    void handleMessage(cMessage *msg) override { delete msg; }
};

Define_Module(Sink);
