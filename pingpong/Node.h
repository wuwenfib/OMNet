// ============================================================
// Node.h — Node 模块的 C++ 头文件
// ============================================================
//
// 在 OMNeT++ 里，每个 simple module 都需要一个对应的 C++ 类。
// 这个类必须继承自 cSimpleModule（或其父类 cModule）。
//
// cSimpleModule 提供了两个核心的"钩子"函数，
// 仿真引擎在适当时机会自动调用它们：
//   · initialize()     仿真开始时调用一次（类似构造函数）
//   · handleMessage()  每次模块收到消息时调用
// ============================================================

// 头文件保护宏，防止被多次 #include 导致重复定义
#ifndef __NODE_H
#define __NODE_H

// omnetpp.h 是 OMNeT++ 的总头文件，包含了所有常用类：
//   cSimpleModule, cMessage, simTime(), EV, scheduleAt(), send() ...
#include <omnetpp.h>

// 引入 omnetpp 命名空间，这样就不用每次都写 omnetpp::cMessage，
// 直接写 cMessage 即可。
using namespace omnetpp;

// Node 类继承自 cSimpleModule。
// cSimpleModule 是所有"简单模块"的基类，提供了消息调度框架。
class Node : public cSimpleModule
{
  protected:
    // initialize() 在仿真启动后、第一个事件处理之前被调用。
    // 适合做：发送第一条消息、初始化变量、读取参数等。
    // override 关键字提示编译器这是对父类虚函数的覆写。
    virtual void initialize() override;

    // handleMessage() 每次模块收到消息（无论来自哪里）时被调用。
    // 参数 msg 是指向收到的消息对象的指针。
    // 注意：消息用完后必须 delete，否则会内存泄漏。
    virtual void handleMessage(cMessage *msg) override;
};

#endif // __NODE_H
