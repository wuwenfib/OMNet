// ============================================================
// Node.cc — 练习 2：使用自定义消息类型 DataPacket
// ============================================================
//
// 与练习 1 相比，新增的内容：
//   1. 创建消息时用 new DataPacket() 代替 new cMessage()
//   2. 用 setter 填写字段：pkt->setSrcAddr(myAddr)
//   3. 收到消息时用 check_and_cast<DataPacket*> 转型，再用 getter 读字段
// ============================================================

#include "Node.h"

Define_Module(Node);

void Node::initialize()
{
    myAddr   = par("myAddr").intValue();
    waitTime = par("waitTime").doubleValue();
    seqNum   = 0;   // 从 0 开始计数

    EV << "【" << getFullName() << "】初始化，地址=" << myAddr
       << "，waitTime=" << waitTime << "s" << endl;

    if (strcmp(getName(), "nodeA") == 0) {
        // 用自消息触发第一次发送（与前两个练习相同）
        scheduleAt(simTime(), new cMessage("start"));
    }
}

void Node::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        // ====================================================
        // 定时器触发：创建并发送一个 DataPacket
        // ====================================================
        delete msg;

        // --------------------------------------------------------
        // 【新概念 1】创建自定义消息
        //
        // new DataPacket("packet") 创建一个 DataPacket 对象。
        // DataPacket 继承自 cMessage，所以它也是一条消息，
        // 可以被 send()、scheduleAt() 等函数接受。
        // --------------------------------------------------------
        DataPacket *pkt = new DataPacket("packet");

        // --------------------------------------------------------
        // 【新概念 2】用 setter 填写字段
        //
        // opp_msgc 为 .msg 里的每个字段生成一对 getter/setter：
        //   int srcAddr  →  getSrcAddr() / setSrcAddr(int v)
        //   int seqNum   →  getSeqNum()  / setSeqNum(int v)
        //
        // 命名规则：set + 首字母大写的字段名
        // --------------------------------------------------------
        pkt->setSrcAddr(myAddr);    // 填入本节点地址
        pkt->setSeqNum(++seqNum);   // 序列号自增后填入

        send(pkt, "gate$o");

        EV << "【t=" << simTime() << "s】【" << getFullName()
           << "】发出 DataPacket #" << seqNum
           << "（srcAddr=" << myAddr << "）" << endl;

    } else {
        // ====================================================
        // 收到来自对方的 DataPacket：读取字段并打印
        // ====================================================

        // --------------------------------------------------------
        // 【新概念 3】check_and_cast<T*> — 安全的向下类型转换
        //
        // 问题：handleMessage 的参数类型是 cMessage*（基类指针），
        //       但实际传来的是 DataPacket*（子类对象）。
        //       如果直接用 (DataPacket*)msg 强制转型，是不安全的——
        //       万一消息实际上不是 DataPacket，会产生未定义行为。
        //
        // check_and_cast<DataPacket*>(msg) 在转型前会检查类型，
        // 如果类型不匹配，会立即报错终止仿真（而不是悄悄出错）。
        // 这是 OMNeT++ 推荐的标准转型方式。
        //
        // 面试题里就是这样把 cMessage* 转成 AppPacket* 的。
        // --------------------------------------------------------
        DataPacket *pkt = check_and_cast<DataPacket *>(msg);

        // --------------------------------------------------------
        // 【新概念 4】用 getter 读取字段
        //
        // 现在 pkt 是 DataPacket* 类型，可以直接调用自定义字段的 getter。
        // --------------------------------------------------------
        EV << "【t=" << simTime() << "s】【" << getFullName()
           << "】收到 DataPacket #" << pkt->getSeqNum()
           << "（来自 srcAddr=" << pkt->getSrcAddr() << "）" << endl;

        // 消息读完就删掉，防止内存泄漏
        delete pkt;

        // 等 waitTime 秒后回复
        scheduleAt(simTime() + waitTime, new cMessage("reply-timer"));
    }
}
