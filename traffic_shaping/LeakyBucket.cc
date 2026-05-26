#include "LeakyBucket.h"
Define_Module(LeakyBucket);

void LeakyBucket::initialize()
{
    bucketSize = par("bucketSize").intValue();
    drainRate  = par("drainRate").doubleValue();
    dropped = forwarded = 0;

    ASSERT(drainRate > 0);
    drainTimer = new cMessage("drain");
    scheduleAt(simTime() + 1.0 / drainRate, drainTimer);
}

void LeakyBucket::handleMessage(cMessage *msg)
{
    if (msg == drainTimer) {
        if (!pktQueue.empty()) {
            DataPacket *pkt = pktQueue.front();
            pktQueue.pop();
            forwarded++;
            send(pkt, "out");
        }
        scheduleAt(simTime() + 1.0 / drainRate, drainTimer);
    } else {
        DataPacket *pkt = check_and_cast<DataPacket *>(msg);
        if ((int)pktQueue.size() < bucketSize) {
            pktQueue.push(pkt);
        } else {
            dropped++;
            delete pkt;
        }
    }
}

void LeakyBucket::finish()
{
    int total = forwarded + dropped;
    double dropRate = total > 0 ? 100.0 * dropped / total : 0.0;

    EV << "\n=============================\n"
       << "=== 漏桶 (LeakyBucket) ===\n"
       << "  转发: " << forwarded << " pkt\n"
       << "  丢弃: " << dropped   << " pkt\n"
       << "  丢包率: " << dropRate << "%\n"
       << "=============================\n";
}

LeakyBucket::~LeakyBucket()
{
    cancelAndDelete(drainTimer);
    while (!pktQueue.empty()) {
        delete pktQueue.front();
        pktQueue.pop();
    }
}
