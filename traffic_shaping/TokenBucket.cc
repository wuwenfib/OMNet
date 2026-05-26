#include <algorithm>
#include "TokenBucket.h"
Define_Module(TokenBucket);

void TokenBucket::initialize()
{
    bucketSize = par("bucketSize").intValue();
    tokenRate  = par("tokenRate").doubleValue();
    tokens     = bucketSize;
    dropped = forwarded = 0;

    ASSERT(tokenRate > 0);
    tokenTimer = new cMessage("token");
    scheduleAt(simTime() + 1.0 / tokenRate, tokenTimer);
}

void TokenBucket::handleMessage(cMessage *msg)
{
    if (msg == tokenTimer) {
        tokens = std::min(tokens + 1, bucketSize);
        scheduleAt(simTime() + 1.0 / tokenRate, tokenTimer);
    } else {
        DataPacket *pkt = check_and_cast<DataPacket *>(msg);
        if (tokens >= 1) {
            tokens -= 1;
            forwarded++;
            send(pkt, "out");
        } else {
            dropped++;
            delete pkt;
        }
    }
}

void TokenBucket::finish()
{
    int total = forwarded + dropped;
    double dropRate = total > 0 ? 100.0 * dropped / total : 0.0;

    EV << "\n=============================\n"
       << "=== 令牌桶 (TokenBucket) ===\n"
       << "  转发: " << forwarded << " pkt\n"
       << "  丢弃: " << dropped   << " pkt\n"
       << "  丢包率: " << dropRate << "%\n"
       << "=============================\n";
}

TokenBucket::~TokenBucket()
{
    cancelAndDelete(tokenTimer);
}
