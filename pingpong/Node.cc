// ============================================================
// Node.cc — Node 模块的行为实现
// ============================================================
//
// 核心概念速览：
//
//  cMessage        OMNeT++ 中传递的"消息"对象，像一个数据包。
//                  可以设置名字、类型，也可以携带自定义数据。
//
//  self-message    自消息：模块发给自己的消息，用来实现"定时器"。
//                  发出后由仿真引擎在指定时间回调 handleMessage()。
//
//  scheduleAt(t,m) 将消息 m 安排在仿真时间 t 送达自己（触发 handleMessage）。
//
//  send(m, gate)   将消息 m 从指定的门发出去，沿连线传到对方节点。
//
//  EV              OMNeT++ 的仿真日志流，输出到控制台/IDE 的事件日志窗口。
//                  用法与 std::cout 相同，但只在仿真环境中有效。
//
//  simTime()       返回当前仿真时间（类型 simtime_t，单位秒）。
// ============================================================

#include "Node.h"

// ============================================================
// Define_Module 宏
// 向 OMNeT++ 仿真引擎注册这个 C++ 类，
// 让引擎知道 NED 文件里的 "Node" 模块应该用哪个 C++ 类来实例化。
// 每个 simple module 的 .cc 文件里必须有且仅有一个 Define_Module。
// ============================================================
Define_Module(Node);


// ============================================================
// initialize() — 仿真启动时调用一次
// ============================================================
void Node::initialize()
{
    // getName() 返回该模块在 NED 里的实例名，即 "nodeA" 或 "nodeB"。
    // 我们用它来区分两个节点，让 nodeA 负责发起第一条消息。
    if (strcmp(getName(), "nodeA") == 0)
    {
        // ---- nodeA 的初始化逻辑 ----
        //
        // 我们需要在仿真一开始就发出第一条消息。
        // 但是 initialize() 里不能直接调用 send()，
        // 因为仿真引擎还没完成所有模块的初始化，
        // 连线还没完全就绪。
        //
        // 正确做法：使用"自消息"(self-message)模拟一个"立即触发的定时器"：
        //   1. 创建一条消息
        //   2. 用 scheduleAt(simTime(), msg) 把它安排在"当前时间"送回自己
        //   3. 仿真引擎在下一个事件循环里调用 handleMessage(msg)
        //   4. 在 handleMessage 里再调用 send()
        //
        // 这样做是 OMNeT++ 的标准惯用法。

        // new cMessage("start") 创建一条名为 "start" 的消息对象。
        // 消息名只是一个标签，方便调试时识别，没有特殊含义。
        cMessage *startMsg = new cMessage("start");

        // scheduleAt(simTime(), startMsg)：
        //   simTime() 返回当前仿真时间（此时为 0s）。
        //   这行的意思是"在 0 秒时把 startMsg 送回我自己"。
        //   结果：仿真开始后的第一个事件就是 nodeA 的 handleMessage(startMsg)。
        scheduleAt(simTime(), startMsg);

        EV << "【nodeA】初始化完成，已安排自消息，将在 t=0 触发首次发送" << endl;
    }
    else
    {
        // ---- nodeB 的初始化逻辑 ----
        // nodeB 什么都不做，被动等待 nodeA 的消息。
        EV << "【nodeB】初始化完成，被动等待 nodeA 的消息..." << endl;
    }
}


// ============================================================
// handleMessage() — 每次收到消息时调用
// ============================================================
//
// OMNeT++ 中"收到消息"有两种来源：
//   1. 来自其他模块（通过连线传过来的普通消息）
//   2. 来自自己（之前用 scheduleAt 安排的自消息，定时器触发）
//
// 用 msg->isSelfMessage() 可以区分这两种情况。
// ============================================================
void Node::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        // ====================================================
        // 分支 A：收到自消息（定时器触发）
        // ====================================================
        //
        // 什么时候会走到这里？
        //   · nodeA 在 initialize() 里 scheduleAt(0, startMsg)  → 触发第一次发送
        //   · 任意节点在收到对方消息后 scheduleAt(t+1, timer)   → 触发回复
        //
        // 无论哪种情况，逻辑都一样：删掉定时器消息，然后发一条新消息给对方。

        EV << "【t=" << simTime() << "s】【" << getFullName()
           << "】定时器触发，准备发消息给对方" << endl;

        // 自消息完成了它的使命（触发定时器），立即释放，避免内存泄漏。
        delete msg;
        msg = nullptr;  // 清空指针，防止后续误用

        // 根据当前节点决定发什么消息名，方便观察日志：
        //   nodeA 发 "ping"，nodeB 发 "pong"
        const char *msgName = (strcmp(getName(), "nodeA") == 0) ? "ping" : "pong";

        // 创建一条新的消息，准备发给对方节点。
        cMessage *outMsg = new cMessage(msgName);

        // send(outMsg, "gate$o")：
        //   从名为 "gate" 的双向门的"输出端"发出消息。
        //   OMNeT++ 把 inout 门在内部拆成两半：
        //     gate$i  — 输入半门，用于接收
        //     gate$o  — 输出半门，用于发送
        //   消息经过连线上设置的 100ms 延迟后，到达对方节点的 gate$i。
        send(outMsg, "gate$o");

        EV << "【t=" << simTime() << "s】【" << getFullName()
           << "】已发出消息 \"" << msgName << "\"（对方 100ms 后收到）" << endl;
    }
    else
    {
        // ====================================================
        // 分支 B：收到来自对方节点的普通消息
        // ====================================================
        //
        // 到这里意味着对方节点通过 send() 发了一条消息，
        // 经过链路延迟（100ms）后送达本节点。

        EV << "【t=" << simTime() << "s】【" << getFullName()
           << "】收到消息 \"" << msg->getName() << "\"，1 秒后回复" << endl;

        // 收到的消息已经用完，必须 delete，否则内存泄漏。
        // 注意：OMNeT++ 不会自动回收消息，由模块负责管理。
        delete msg;
        msg = nullptr;

        // 现在安排一个"1 秒后触发的定时器"，到时候再发回复。
        // 这就是 OMNeT++ 里实现延迟操作的标准方式：
        //   不能用 sleep()！sleep 会冻结整个仿真引擎。
        //   必须用 scheduleAt() + 自消息来异步延迟。

        cMessage *replyTimer = new cMessage("reply-timer");

        // simTime() + 1.0 表示"当前仿真时间再加 1 秒"。
        // 仿真时间是离散事件驱动的，不是真实时间，
        // 所以这里的"1 秒"是仿真世界里的 1 秒，
        // 在真实计算机上可能只需要几毫秒就能算完。
        scheduleAt(simTime() + 1.0, replyTimer);

        EV << "【t=" << simTime() << "s】【" << getFullName()
           << "】已安排回复定时器，将在 t=" << (simTime() + 1.0) << "s 发出回复" << endl;
    }
}
