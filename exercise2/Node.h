// ============================================================
// Node.h — 练习 2
// ============================================================
#ifndef __NODE_H
#define __NODE_H

#include <omnetpp.h>
// --------------------------------------------------------
// 【新增】引入自动生成的消息头文件
//
// DataPacket.msg 经 opp_msgc 编译后生成 DataPacket_m.h，
// 里面包含 DataPacket 类的完整定义。
// 注意：文件名后缀固定是 _m.h，不是 .h。
// --------------------------------------------------------
#include "DataPacket_m.h"

using namespace omnetpp;

class Node : public cSimpleModule
{
  protected:
    int    myAddr;    // 本节点地址，从 NED 参数读入
    double waitTime;  // 等待时间，从 NED 参数读入
    int    seqNum;    // 发送计数器，记录发出了第几条消息

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
