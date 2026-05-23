#ifndef QUEUESERVER_H_
#define QUEUESERVER_H_

#include <omnetpp.h>
#include <queue>
#include "Job_m.h"

using namespace omnetpp;

class QueueServer : public cSimpleModule {
  private:
    // ---- NED 参数 ----
    double serviceTimeMean;
    int    queueCapacity;

    // ---- 运行时状态 ----
    bool serverBusy;                  // 服务器是否正在服务
    std::queue<Job*> waitingQueue;    // FIFO 等待队列
    int  currentQueueLength;          // 队列当前长度（镜像 waitingQueue.size()，用于 WATCH）
    Job *jobInService;                // 当前正在服务的 Job
    cMessage *endServiceMsg;          // 自消息：服务完成定时器

    // ---- 统计累积量 ----
    long arrivedCount;                // 到达总数
    long droppedCount;                // 丢弃总数
    int  maxQueueLength;              // 历史最大队长
    double queueLengthArea;           // 队长对时间的积分（算时间加权平均）
    simtime_t lastQueueChangeTime;    // 上次队长变化的时刻
    double totalBusyTime;             // 服务器累计忙时
    simtime_t busyStartTime;          // 本轮"忙"的起始时刻
    double queueingDelaySum;          // 排队延迟总和
    long   queueingDelayCount;        // 参与平均的样本数

    // ---- 辅助函数 ----
    void recordQueueLengthArea();     // 在队长即将变化前结算积分面积
    void startService(Job *job);      // 让 job 进入服务并排服务完成定时器

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

  public:
    virtual ~QueueServer();
};

#endif
