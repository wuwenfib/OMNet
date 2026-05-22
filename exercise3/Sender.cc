#include "Sender.h"
Define_Module(Sender);

void Sender::initialize()
{
    srcAddr  = par("srcAddr").intValue();
    interval = par("interval").doubleValue();
    counter  = 0;

    // 立即触发第一次发包
    scheduleAt(simTime(), new cMessage("tick"));
}

void Sender::handleMessage(cMessage *msg)
{
    // msg 是定时器自消息
    delete msg;

    // 交替选择 destIndex：0, 1, 0, 1, ...
    int destIndex = counter % 2;
    counter++;

    DataPacket *pkt = new DataPacket("pkt");
    pkt->setSrcAddr(srcAddr);
    pkt->setDestIndex(destIndex);

    EV << "【t=" << simTime() << "s】Sender 发包 #" << counter
       << " → destIndex=" << destIndex << endl;

    send(pkt, "out");

    // 安排下一次发包
    scheduleAt(simTime() + interval, new cMessage("tick"));
}
