#include "Sender.h"
Define_Module(Sender);

// 依次发这 4 个目的地址，覆盖 Router 的所有分支
static const int DEST_SEQUENCE[] = {10, 20, 5, 99};
static const int SEQ_LEN = 4;

void Sender::initialize()
{
    srcAddr  = par("srcAddr").intValue();
    interval = par("interval").doubleValue();
    counter  = 0;
    scheduleAt(simTime(), new cMessage("tick"));
}

void Sender::handleMessage(cMessage *msg)
{
    delete msg;

    if (counter >= SEQ_LEN) return;   // 4 个包发完就停

    int dest = DEST_SEQUENCE[counter++];

    DataPacket *pkt = new DataPacket("pkt");
    pkt->setSrcAddr(srcAddr);
    pkt->setDestAddr(dest);

    EV << "【t=" << simTime() << "s】Sender 发包 #" << counter
       << "  srcAddr=" << srcAddr << "  destAddr=" << dest << endl;

    send(pkt, "out");
    scheduleAt(simTime() + interval, new cMessage("tick"));
}
