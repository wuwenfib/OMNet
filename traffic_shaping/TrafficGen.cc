#include "TrafficGen.h"
Define_Module(TrafficGen);

void TrafficGen::initialize()
{
    onTime   = par("onTime").doubleValueInUnit("s");
    offTime  = par("offTime").doubleValueInUnit("s");
    sendRate = par("sendRate").intValue();
    pktSize  = par("pktSize").intValue();
    seqNum   = 0;
    totalSent = 0;

    ASSERT(sendRate > 0);
    onMsg   = new cMessage("on");
    offMsg  = new cMessage("off");
    tickMsg = new cMessage("tick");

    scheduleAt(0, onMsg);
}

void TrafficGen::handleMessage(cMessage *msg)
{
    if (msg == onMsg) {
        scheduleAt(simTime() + 1.0 / sendRate, tickMsg);
        scheduleAt(simTime() + onTime, offMsg);

    } else if (msg == offMsg) {
        cancelEvent(tickMsg);
        scheduleAt(simTime() + offTime, onMsg);

    } else if (msg == tickMsg) {
        DataPacket *pkt1 = new DataPacket("pkt");
        pkt1->setSeqNum(seqNum);
        pkt1->setSendTime(simTime().dbl());
        pkt1->setSizeBytes(pktSize);

        DataPacket *pkt2 = pkt1->dup();

        send(pkt1, "outTB");
        send(pkt2, "outLB");

        seqNum++;
        totalSent++;

        scheduleAt(simTime() + 1.0 / sendRate, tickMsg);
    }
}

void TrafficGen::finish()
{
    EV << "\n=============================\n"
       << "=== TrafficGen 统计 ===\n"
       << "  发包总数(每路): " << totalSent << " pkt\n"
       << "=============================\n";
}

TrafficGen::~TrafficGen()
{
    cancelAndDelete(onMsg);
    cancelAndDelete(offMsg);
    cancelAndDelete(tickMsg);
}
