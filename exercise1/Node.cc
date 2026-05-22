// ============================================================
// Node.cc — 练习 1：用 par() 读取 NED 参数
// ============================================================
//
// 与 pingpong/Node.cc 相比，只改了两处：
//   1. initialize() 里新增：waitTime = par("waitTime").doubleValue();
//   2. handleMessage() 里把硬编码的 1.0 换成 waitTime
//
// 其余逻辑与 pingpong 完全相同，方便对比。
// ============================================================

#include "Node.h"

Define_Module(Node);

void Node::initialize()
{
    // ============================================================
    // 【新概念】par("参数名") — 读取 NED 参数
    // ============================================================
    //
    // par("waitTime") 返回一个 cPar 对象，代表名为 "waitTime" 的参数。
    // 必须调用对应类型的转换方法才能拿到实际值：
    //
    //   par("waitTime").doubleValue()  → double（浮点数）
    //   par("address").intValue()      → int（整数）    ← 面试题里会用到！
    //   par("name").stringValue()      → const char*
    //   par("enabled").boolValue()     → bool
    //
    // 也可以直接隐式转换：double w = par("waitTime");  效果相同。
    //
    // par() 读取的值的来源（按优先级从高到低）：
    //   1. omnetpp.ini  里的设置（最高）
    //   2. Network.ned  里给子模块的赋值
    //   3. Node.ned     里的 default(...)（最低）
    //
    // 本例中：
    //   ini 里设置了 *.nodeA.waitTime = 0.5
    //            和 *.nodeB.waitTime = 1.5
    //   所以 nodeA 读到 0.5，nodeB 读到 1.5，Network.ned 里的 0.8 被覆盖。
    // ============================================================
    waitTime = par("waitTime").doubleValue();

    EV << "【" << getFullName() << "】初始化完成，waitTime = "
       << waitTime << " 秒" << endl;

    if (strcmp(getName(), "nodeA") == 0) {
        cMessage *startMsg = new cMessage("start");
        scheduleAt(simTime(), startMsg);
    }
}

void Node::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        EV << "【t=" << simTime() << "s】【" << getFullName()
           << "】定时器触发，发消息给对方" << endl;

        delete msg;

        const char *msgName = (strcmp(getName(), "nodeA") == 0) ? "ping" : "pong";
        cMessage *outMsg = new cMessage(msgName);
        send(outMsg, "gate$o");

        EV << "【t=" << simTime() << "s】【" << getFullName()
           << "】发出 \"" << msgName << "\"" << endl;

    } else {
        EV << "【t=" << simTime() << "s】【" << getFullName()
           << "】收到 \"" << msg->getName()
           << "\"，等待 " << waitTime << " 秒后回复" << endl;

        delete msg;

        cMessage *timer = new cMessage("reply-timer");

        // --------------------------------------------------------
        // 关键变化：原来是 scheduleAt(simTime() + 1.0, timer)
        // 现在换成   scheduleAt(simTime() + waitTime, timer)
        // waitTime 的值来自 NED 参数，不再硬编码。
        // --------------------------------------------------------
        scheduleAt(simTime() + waitTime, timer);
    }
}
