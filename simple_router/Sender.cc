#include "Sender.h"

Define_Module(Sender);

// 四个目的地址，刻意覆盖 SimpleRouter 的全部三条分支：
//   5  -> 等于路由器本机地址  => [到达]
//   10 -> 命中路由表 out[0]   => [转发]
//   20 -> 命中路由表 out[1]   => [转发]
//   99 -> 路由表中没有        => [丢包]
static const int DEST_LIST[] = {5, 10, 20, 30,99};
static const int DEST_COUNT  = 5;

void Sender::initialize()
{
    srcAddr  = par("srcAddr").intValue();
    interval = par("interval").doubleValue();
    sendIndex = 0;

    // 用一个自消息（定时器）触发第一次发包
    scheduleAt(simTime() + interval, new cMessage("sendTimer"));
}

void Sender::handleMessage(cMessage *msg)
{
    // 这里收到的只会是自己安排的定时器自消息
    if (sendIndex < DEST_COUNT) {
        int dest = DEST_LIST[sendIndex];

        // 构造一个 AppPacket，填好源地址与目的地址
        AppPacket *pkt = new AppPacket("AppPacket");
        pkt->setSrcAddr(srcAddr);
        pkt->setDestAddr(dest);

        EV << "[Sender] 发出数据包 #" << sendIndex
           << "  srcAddr=" << srcAddr << "  destAddr=" << dest << endl;
        send(pkt, "out");

        sendIndex++;
    }

    // 还有剩余数据包就再排一个定时器，否则释放定时器消息
    if (sendIndex < DEST_COUNT)
        scheduleAt(simTime() + interval, msg);
    else
        delete msg;
}
