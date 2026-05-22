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

    // (2) 手动硬编码一张静态路由表（题目允许，便于测试）
    //     key   = 目的地址 destAddr
    //     value = 对应的输出门索引 out[idx]
    routingTable[10] = 0;   // 目的地址 10 -> 从 out[0] 转发
    routingTable[20] = 1;   // 目的地址 20 -> 从 out[1] 转发

    EV << "[SimpleRouter] 启动完成，本机地址 = " << myAddr
       << "，路由表 = {10->out[0], 20->out[1]}" << endl;
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
        EV << "[转发] 数据包目的地址 " << dest
           << " 命中路由表，从 out[" << outIdx << "] 转发" << endl;
        send(pkt, "out", outIdx);         // 转发后所有权移交内核，不能再 delete
    }
    else {
        // 分支三 —— 路由失败（丢包）：表中无对应条目
        EV << "[丢包] 数据包目的地址 " << dest
           << " 在路由表中无对应条目，丢弃" << endl;
        delete pkt;                       // 丢弃同样要释放内存
    }
}
