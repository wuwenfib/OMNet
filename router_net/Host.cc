#include "Host.h"
#include <sstream>
#include <iostream>

Define_Module(Host);

void Host::parseDestinations(const std::string &spec)
{
    std::stringstream ss(spec);
    std::string item;
    while (std::getline(ss, item, ',')) {
        auto l = item.find_first_not_of(" \t");
        auto r = item.find_last_not_of(" \t");
        if (l == std::string::npos) continue;
        item = item.substr(l, r - l + 1);
        destinations.push_back(std::stoi(item));
    }
}

void Host::initialize()
{
    myAddr           = par("address").intValue();
    interArrivalMean = par("interArrivalMean").doubleValue();
    initialTtl       = par("initialTtl").intValue();
    parseDestinations(par("destinations").stdstringValue());

    nextSerial = 0;
    sendTimer  = nullptr;

    sigGenerated = registerSignal("generated");
    sigDelivered = registerSignal("delivered");

    // 只有给出 destinations 的主机才会发包；其他主机当纯接收端
    if (!destinations.empty()) {
        sendTimer = new cMessage("sendTimer");
        scheduleAt(simTime() + exponential(interArrivalMean), sendTimer);
    }

    EV << "[Host-" << myAddr << "] 启动，目的地数="
       << destinations.size() << "，initialTtl=" << initialTtl << endl;
}

void Host::handleMessage(cMessage *msg)
{
    if (msg == sendTimer) {
        // -------- 发包分支 --------
        int dest = destinations[intuniform(0, destinations.size() - 1)];

        AppPacket *pkt = new AppPacket("AppPacket");
        pkt->setSrcAddr(myAddr);
        pkt->setDestAddr(dest);
        pkt->setTtl(initialTtl);
        pkt->setSerial(++nextSerial);

        EV << "[Host-" << myAddr << "] 发出 #" << nextSerial
           << "  dest=" << dest << "  ttl=" << initialTtl << endl;
        emit(sigGenerated, 1L);
        send(pkt, "port$o");

        // 排下一次发包
        scheduleAt(simTime() + exponential(interArrivalMean), sendTimer);
        return;
    }

    // -------- 收包分支 --------
    AppPacket *pkt = check_and_cast<AppPacket *>(msg);
    if (pkt->getDestAddr() != myAddr) {
        // 不应该发生：路由表错把别人的包送到我这里
        EV_WARN << "[Host-" << myAddr << "] 收到非本机包 dest="
                << pkt->getDestAddr() << "（路由配置异常）" << endl;
        delete pkt;
        return;
    }

    EV << "[Host-" << myAddr << "] 收到 #" << pkt->getSerial()
       << "  src=" << pkt->getSrcAddr() << "  hopCount=" << pkt->getHopCount() << endl;
    emit(sigDelivered, (long)pkt->getHopCount());
    delete pkt;
}

void Host::finish()
{
    // express 模式下 EV 被屏蔽，统计汇总用 std::cout 无条件打印
    std::cout << "[Host-" << myAddr << "] finish: 发出 " << nextSerial << " 个包" << std::endl;
}

Host::~Host()
{
    if (sendTimer)
        cancelAndDelete(sendTimer);
}
