#include "Router.h"
#include <sstream>

Define_Module(Router);

// ------------------------------------------------------------
// 字符串 → 路由表
// 期望格式："destAddr:portIdx,destAddr:portIdx,..."，空白可有可无
// ------------------------------------------------------------
void Router::parseRoutingTable(const std::string &spec)
{
    std::stringstream ss(spec);
    std::string item;
    while (std::getline(ss, item, ',')) {
        // 去除前后空白
        auto l = item.find_first_not_of(" \t");
        auto r = item.find_last_not_of(" \t");
        if (l == std::string::npos) continue;
        item = item.substr(l, r - l + 1);

        auto colon = item.find(':');
        if (colon == std::string::npos)
            throw cRuntimeError("路由表条目格式错误（缺少冒号）: '%s'", item.c_str());

        int dest = std::stoi(item.substr(0, colon));
        int port = std::stoi(item.substr(colon + 1));
        routingTable[dest] = port;
    }
}

void Router::initialize()
{
    myAddr      = par("address").intValue();
    defaultPort = par("defaultPort").intValue();
    parseRoutingTable(par("routingTable").stdstringValue());

    // 注册信号 —— 必须用 registerSignal，emit 时才能找到接收侧
    sigForwarded      = registerSignal("forwarded");
    sigDroppedNoRoute = registerSignal("droppedNoRoute");
    sigTtlExpired     = registerSignal("ttlExpired");

    // 打印路由表，方便核对
    EV << "[Router-" << myAddr << "] 启动，路由表 = {";
    for (auto &kv : routingTable)
        EV << " " << kv.first << "->port[" << kv.second << "]";
    EV << " } defaultPort=" << defaultPort << endl;
}

void Router::handleMessage(cMessage *msg)
{
    AppPacket *pkt = check_and_cast<AppPacket *>(msg);

    // ---------- TTL 检查 ----------
    int ttl = pkt->getTtl() - 1;
    pkt->setTtl(ttl);
    pkt->setHopCount(pkt->getHopCount() + 1);

    if (ttl <= 0) {
        EV << "[Router-" << myAddr << "] TTL 耗尽，丢弃 #" << pkt->getSerial()
           << "（dest=" << pkt->getDestAddr() << "）" << endl;
        emit(sigTtlExpired, 1L);
        delete pkt;
        return;
    }

    // ---------- 查表 ----------
    int dest = pkt->getDestAddr();
    auto it = routingTable.find(dest);

    int outPort = -1;
    if (it != routingTable.end()) {
        outPort = it->second;
    }
    else if (defaultPort >= 0) {
        outPort = defaultPort;
        EV << "[Router-" << myAddr << "] dest=" << dest
           << " 未命中，走默认出口 port[" << outPort << "]" << endl;
    }

    if (outPort < 0) {
        EV << "[Router-" << myAddr << "] dest=" << dest
           << " 未命中且无默认出口，丢弃 #" << pkt->getSerial() << endl;
        emit(sigDroppedNoRoute, 1L);
        delete pkt;
        return;
    }

    // ---------- 转发 ----------
    EV << "[Router-" << myAddr << "] 转发 #" << pkt->getSerial()
       << "  dest=" << dest << "  ttl=" << ttl
       << " -> port[" << outPort << "]" << endl;
    emit(sigForwarded, 1L);
    send(pkt, "port$o", outPort);
}
