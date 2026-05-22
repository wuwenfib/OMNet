// ============================================================
// Router.cc — 练习 4 核心：std::map 路由表 + 三路分支
//
// 这就是面试题 SimpleRouter 的完整逻辑，换了个名字而已。
// ============================================================
#include "Router.h"
Define_Module(Router);

void Router::initialize()
{
    myAddr = par("myAddr").intValue();

    // --------------------------------------------------------
    // 【新概念 1】手动硬编码路由表
    //
    // std::map 的下标运算符 [] 既可以插入也可以更新：
    //   routingTable[目的地址] = 输出门索引;
    //
    // 这里的设置和面试题完全一致：
    //   目的地址 10 → 从 out[0] 发出
    //   目的地址 20 → 从 out[1] 发出
    // --------------------------------------------------------
    routingTable[10] = 0;
    routingTable[20] = 1;

    EV << "【Router】初始化，myAddr=" << myAddr
       << "，路由表条目数=" << routingTable.size() << endl;
}

void Router::handleMessage(cMessage *msg)
{
    // 安全转型：cMessage* → DataPacket*
    // （面试题里是 AppPacket*，用法完全相同）
    DataPacket *pkt = check_and_cast<DataPacket *>(msg);

    int src  = pkt->getSrcAddr();
    int dest = pkt->getDestAddr();

    EV << "【t=" << simTime() << "s】Router 收到包  srcAddr="
       << src << "  destAddr=" << dest << "  →  ";

    // ====================================================
    // 三路分支：到达 / 转发 / 丢包
    // 面试题的 handleMessage 就是这个结构
    // ====================================================

    if (dest == myAddr) {
        // ------------------------------------------------
        // 分支 1：包的目的地就是本机，到达终点
        // ------------------------------------------------
        EV << "到达本机（myAddr=" << myAddr
           << "），来自 srcAddr=" << src << endl;
        delete pkt;   // 消费掉，释放内存

    } else {
        // ------------------------------------------------
        // 分支 2 & 3：需要转发，先查路由表
        //
        // 【新概念 2】map::find() — 安全查找
        //
        // find(key) 返回一个迭代器：
        //   · 找到了 → 迭代器指向该条目，it->second 是值（门索引）
        //   · 没找到 → 迭代器等于 routingTable.end()（哨兵值）
        //
        // 不要用 routingTable[dest]，因为找不到时它会自动插入一条
        // 值为 0 的条目，污染路由表，并且无法区分"找到了索引0"
        // 和"没找到自动插入了0"这两种情况。
        // ------------------------------------------------
        auto it = routingTable.find(dest);

        if (it != routingTable.end()) {
            // 分支 2：查到了路由条目
            int outIndex = it->second;   // it->second 是 map 的值（门索引）

            EV << "查表成功，从 out[" << outIndex << "] 转发" << endl;
            send(pkt, "out", outIndex);  // 三参数 send，从指定门发出

        } else {
            // 分支 3：路由表里没有这个目的地址，丢包
            EV << "查表失败（destAddr=" << dest
               << " 无路由条目），丢包" << endl;
            delete pkt;   // 必须 delete，否则内存泄漏
        }
    }
}
