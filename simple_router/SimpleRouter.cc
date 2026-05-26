#include "SimpleRouter.h"

// ============================================================
// 题目要求 1：类定义与注册
// Define_Module 宏把 C++ 类 SimpleRouter 与 NED 里的
// "simple SimpleRouter" 绑定，仿真内核才能在建网时实例化它。
// ============================================================
Define_Module(SimpleRouter);

// ============================================================
// 题目要求 2：初始化逻辑 initialize()
// 仿真开始、各模块建好后，内核对每个模块调用一次 initialize()。
// ============================================================
void SimpleRouter::initialize()
{
    // (1) 从 NED 参数读取本机地址，存为成员变量
    //     参数名固定为 "address"，值由 omnetpp.ini 注入
    myAddr = par("address").intValue();

    // (2) 从 NED 参数读取路由表字符串，由 omnetpp.ini 注入
    //     格式："destAddr:gateIdx" 多条用空格或逗号分隔，例如 "10:0 20:1"
    //     用 cStringTokenizer 做两层切分：先按空格/逗号切条目，再按冒号切 K:V
    const char *rtStr = par("routingTable").stringValue();
    cStringTokenizer entries(rtStr, " ,");
    while (entries.hasMoreTokens()) {
        const char *entry = entries.nextToken();
        cStringTokenizer kv(entry, ":");
        if (!kv.hasMoreTokens())
            continue;                                  // 跳过空条目
        int dest = atoi(kv.nextToken());
        if (!kv.hasMoreTokens())
            throw cRuntimeError("routingTable 条目格式错误（缺少 gateIdx）: \"%s\"", entry);
        int gate = atoi(kv.nextToken());

        // gate 范围校验：out[] 是动态门阵列，越界会让 send() 崩溃
        if (gate < 0 || gate >= gateSize("out"))
            throw cRuntimeError("routingTable 条目 \"%s\" 的 gateIdx 越界，out[] 大小为 %d",
                                entry, gateSize("out"));

        routingTable[dest] = gate;
    }

    EV << "[SimpleRouter] 启动完成，本机地址 = " << myAddr
       << "，加载路由条目 " << routingTable.size() << " 条:" << endl;
    for (auto& kv : routingTable)
        EV << "    " << kv.first << " -> out[" << kv.second << "]" << endl;

    // (3) 注册信号：字符串名 -> 整数 ID。
    //     名字必须和 SimpleRouter.ned 里 @signal[xxx] 方括号内的名字完全一致
    sigDelivered = registerSignal("delivered");
    sigForwarded = registerSignal("forwarded");
    sigDropped   = registerSignal("dropped");
}

// ============================================================
// 题目要求 3：消息处理逻辑 handleMessage()
// 每收到一个消息，内核就回调一次 handleMessage()。
// ============================================================
void SimpleRouter::handleMessage(cMessage *msg)
{
    // (1) 安全地把通用消息转换为 AppPacket 类型
    //     check_and_cast：转换失败会直接报错终止，比 dynamic_cast 更安全
    AppPacket *pkt = check_and_cast<AppPacket *>(msg);

    int src  = pkt->getSrcAddr();
    int dest = pkt->getDestAddr();

    // (2) 分支一 —— 到达终点：目的地址 == 本机地址
    if (dest == myAddr) {
        emit(sigDelivered, (long)dest);   // 信号值 = dest，便于 vector 记录
        deliveredCount++;
        EV << "[到达] 收到来自 " << src << " 的数据包，目的地址 "
           << dest << " == 本机地址，已抵达终点" << endl;
        delete pkt;                       // 妥善处理生命周期，防止内存泄漏
        return;
    }

    // (3) 不是发给本机 —— 查路由表
    auto it = routingTable.find(dest);    // find 不会像 [] 那样误插入新条目

    if (it != routingTable.end()) {
        // 分支二 —— 路由成功：找到条目，按门索引转发
        int outIdx = it->second;
        emit(sigForwarded, (long)dest);
        forwardedCount++;
        EV << "[转发] 数据包目的地址 " << dest
           << " 命中路由表，从 out[" << outIdx << "] 转发" << endl;
        send(pkt, "out", outIdx);         // 转发后所有权移交内核，不能再 delete
    }
    else {
        // 分支三 —— 路由失败（丢包）：表中无对应条目
        emit(sigDropped, (long)dest);
        droppedCount++;
        EV << "[丢包] 数据包目的地址 " << dest
           << " 在路由表中无对应条目，丢弃" << endl;
        delete pkt;                       // 丢弃同样要释放内存
    }
}

// ============================================================
// finish() —— 仿真结束时被内核调用一次（自然终止 / sim-time-limit）
// 仅用于在控制台直观打印汇总；统计真正落盘是 @statistic 干的活
// ============================================================
void SimpleRouter::finish()
{
    long total = deliveredCount + forwardedCount + droppedCount;
    double dropRate = (total > 0) ? (double)droppedCount / total : 0.0;

    EV_INFO << "========== SimpleRouter@addr=" << myAddr << " 汇总 ==========" << endl;
    EV_INFO << "  收到包总数 = " << total          << endl;
    EV_INFO << "  到达本机   = " << deliveredCount << endl;
    EV_INFO << "  转发       = " << forwardedCount << endl;
    EV_INFO << "  丢弃       = " << droppedCount   << endl;
    EV_INFO << "  丢包率     = " << dropRate       << endl;
    EV_INFO << "  详细结果见 results/*.sca / *.vec" << endl;
    EV_INFO << "===========================================" << endl;
}
