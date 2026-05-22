#include "Receiver.h"

Define_Module(Receiver);

void Receiver::initialize()
{
    myAddr = par("address").intValue();
}

void Receiver::handleMessage(cMessage *msg)
{
    AppPacket *pkt = check_and_cast<AppPacket *>(msg);
    EV << "[Receiver-" << myAddr << "] 收到数据包  srcAddr="
       << pkt->getSrcAddr() << "  destAddr=" << pkt->getDestAddr() << endl;
    delete pkt;     // 终端节点收下后释放内存
}
