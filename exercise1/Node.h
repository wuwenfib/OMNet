// ============================================================
// Node.h — 练习 1：新增成员变量存储参数值
// ============================================================
//
// 与 pingpong 相比，唯一的变化：
//   新增成员变量 waitTime，在 initialize() 里从 par() 读入，
//   在 handleMessage() 里用它替换原来硬编码的 1.0。
// ============================================================

#ifndef __NODE_H
#define __NODE_H

#include <omnetpp.h>
using namespace omnetpp;

class Node : public cSimpleModule
{
  protected:
    // 【新增】存储从 NED 参数读入的等待时间。
    // 在 initialize() 里赋值一次，之后在 handleMessage() 里重复使用。
    // 类型用 double 与 NED 参数类型保持一致。
    double waitTime;

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
