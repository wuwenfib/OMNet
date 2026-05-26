#include "Sink.h"
Define_Module(Sink);

void Sink::initialize()
{
    label      = par("label").stdstringValue();
    received   = 0;
    totalDelay = 0.0;
    maxDelay   = 0.0;
    startTime  = simTime();
}

void Sink::handleMessage(cMessage *msg)
{
    DataPacket *pkt = check_and_cast<DataPacket *>(msg);
    double delay = simTime().dbl() - pkt->getSendTime();
    totalDelay += delay;
    if (delay > maxDelay) maxDelay = delay;
    received++;
    delete pkt;
}

void Sink::finish()
{
    double elapsed    = (simTime() - startTime).dbl();
    double avgDelay   = received > 0 ? totalDelay / received : 0.0;
    double throughput = elapsed  > 0 ? received   / elapsed  : 0.0;

    EV << "\n=============================\n"
       << "=== Sink [" << label << "] ===\n"
       << "  收到包数: " << received    << " pkt\n"
       << "  平均延迟: " << avgDelay    << " s\n"
       << "  最大延迟: " << maxDelay    << " s\n"
       << "  吞吐量:   " << throughput  << " pkt/s\n"
       << "=============================\n";
}
