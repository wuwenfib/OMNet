#ifndef __ROUTER_H
#define __ROUTER_H

#include <omnetpp.h>
#include <map>           // 【新增】STL map，用于路由表
#include "DataPacket_m.h"
using namespace omnetpp;

class Router : public cSimpleModule
{
  protected:
    int myAddr;

    // --------------------------------------------------------
    // 【新概念】std::map<int, int> 路由表
    //
    // map 是 C++ STL 的有序关联容器，存储 键→值 的映射。
    // 这里：键 = 目的地址（destAddr），值 = 输出门索引
    //
    // 常用操作：
    //   routingTable[10] = 0;          插入/更新
    //   routingTable.find(key)         查找，返回迭代器
    //   routingTable.end()             "找不到"的标志
    //   it->first / it->second         键 / 值
    // --------------------------------------------------------
    std::map<int, int> routingTable;   // destAddr → 输出门索引

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};
#endif
