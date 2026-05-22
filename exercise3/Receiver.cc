#include "Receiver.h"
Define_Module(Receiver);

void Receiver::handleMessage(cMessage *msg)
{
    DataPacket *pkt = check_and_cast<DataPacket *>(msg);

    EV << "【t=" << simTime() << "s】【" << getFullName()
       << "】收到包（srcAddr=" << pkt->getSrcAddr()
       << "，destIndex=" << pkt->getDestIndex() << "）" << endl;

    delete pkt;
}
