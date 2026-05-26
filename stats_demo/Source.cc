#include "Source.h"

Define_Module(Source);

void Source::initialize()
{
    numPackets = par("numPackets").intValue();
    interval   = par("interval").doubleValue();
    sent       = 0;

    timer = new cMessage("tick");
    scheduleAt(simTime() + interval, timer);
}

void Source::handleMessage(cMessage *msg)
{
    // timer 自消息触发 -> 发一个包，包用 kind 字段携带"序号"
    auto *pkt = new cMessage("pkt");
    pkt->setKind(sent);              // Filter 端会把它当统计值 emit 出去
    send(pkt, "out");
    sent++;

    if (sent < numPackets) {
        scheduleAt(simTime() + interval, msg);
    } else {
        // 发完最后一个包：让析构函数统一回收 timer，这里不要双删
    }
}
