#include "QueueServer.h"
#include <algorithm>
#include <iostream>

Define_Module(QueueServer);

void QueueServer::initialize()
{
    serviceTimeMean = par("serviceTimeMean").doubleValue();
    queueCapacity   = par("queueCapacity").intValue();

    serverBusy         = false;
    currentQueueLength = 0;
    jobInService       = nullptr;
    endServiceMsg      = new cMessage("endService");

    arrivedCount        = 0;
    droppedCount        = 0;
    maxQueueLength      = 0;
    queueLengthArea     = 0;
    lastQueueChangeTime = simTime();
    totalBusyTime       = 0;
    queueingDelaySum    = 0;
    queueingDelayCount  = 0;

    // 把这些变量注册给 Qtenv，Inspector 里就能实时看到它们的当前值。
    WATCH(serverBusy);
    WATCH(currentQueueLength);
    WATCH(arrivedCount);
    WATCH(droppedCount);
    WATCH(maxQueueLength);
    WATCH(queueLengthArea);
    WATCH(totalBusyTime);
    WATCH(queueingDelaySum);
    WATCH(queueingDelayCount);
}

// 在队长即将变化（入队/出队）之前调用：
// 把"上一次变化到现在这段时间 × 当时的队长"累加进积分面积。
void QueueServer::recordQueueLengthArea()
{
    simtime_t now = simTime();
    queueLengthArea += waitingQueue.size() * (now - lastQueueChangeTime).dbl();
    lastQueueChangeTime = now;
}

// 让 job 进入服务：记录排队延迟，并排一个服务完成定时器。
void QueueServer::startService(Job *job)
{
    jobInService = job;

    // 排队延迟 = 现在（开始服务）− 该 job 被创建的时刻
    double delay = (simTime() - job->getCreationTime()).dbl();
    queueingDelaySum += delay;
    queueingDelayCount++;

    // 服务时长服从指数分布，这就是排队论里的服务过程 "M"
    scheduleAt(simTime() + exponential(serviceTimeMean), endServiceMsg);
}

void QueueServer::handleMessage(cMessage *msg)
{
    if (msg == endServiceMsg) {
        // ================= 事件 B：服务完成 =================
        EV << "[QueueServer] Job #" << jobInService->getJobId()
           << " 服务完成，发往 Sink" << endl;
        send(jobInService, "out");
        jobInService = nullptr;

        if (!waitingQueue.empty()) {
            // 队列里还有人在等：取队首继续服务
            recordQueueLengthArea();
            Job *next = waitingQueue.front();
            waitingQueue.pop();
            currentQueueLength = waitingQueue.size();
            startService(next);
        }
        else {
            // 没人等了：服务器转入空闲，结算这一轮忙时
            serverBusy = false;
            totalBusyTime += (simTime() - busyStartTime).dbl();
        }
    }
    else {
        // ================= 事件 A：新 Job 到达 =================
        Job *job = check_and_cast<Job*>(msg);
        arrivedCount++;

        if (!serverBusy) {
            // 服务器空闲 -> 直接开服务（排队延迟为 0）
            serverBusy    = true;
            busyStartTime = simTime();
            startService(job);
        }
        else if ((int)waitingQueue.size() < queueCapacity) {
            // 服务器忙、队列未满 -> 入队等待
            recordQueueLengthArea();
            waitingQueue.push(job);
            currentQueueLength = waitingQueue.size();
            maxQueueLength = std::max(maxQueueLength, currentQueueLength);
        }
        else {
            // 服务器忙、队列已满 -> 丢弃
            droppedCount++;
            EV << "[QueueServer] 队列已满，丢弃 Job #" << job->getJobId() << endl;
            delete job;
        }
    }
}

void QueueServer::finish()
{
    // 结算最后一段队长积分面积
    recordQueueLengthArea();
    // 若仿真结束时服务器仍在忙，补上最后这段未结算的忙时
    if (serverBusy)
        totalBusyTime += (simTime() - busyStartTime).dbl();

    double dur = simTime().dbl();

    // 用 std::cout：express 模式下 EV 被屏蔽，统计汇总必须无条件打印。
    std::cout << "\n==== QueueServer 统计 ====" << std::endl;
    std::cout << "  到达总数       : " << arrivedCount << std::endl;
    std::cout << "  丢弃总数       : " << droppedCount << std::endl;
    std::cout << "  丢包率         : " << (arrivedCount > 0 ? (double)droppedCount / arrivedCount : 0.0) << std::endl;
    std::cout << "  平均队长       : " << (dur > 0 ? queueLengthArea / dur : 0.0) << std::endl;
    std::cout << "  最大队长       : " << maxQueueLength << std::endl;
    std::cout << "  服务器利用率   : " << (dur > 0 ? totalBusyTime / dur : 0.0) << std::endl;
    std::cout << "  平均排队延迟   : " << (queueingDelayCount > 0 ? queueingDelaySum / queueingDelayCount : 0.0) << "s" << std::endl;
    std::cout << "  结束时队列残留 : " << waitingQueue.size() << std::endl;
    std::cout << "  结束时服务器   : " << (jobInService ? "仍在服务 1 个 Job" : "空闲") << std::endl;
}

QueueServer::~QueueServer()
{
    // 清理自消息定时器
    cancelAndDelete(endServiceMsg);
    // 清理仿真结束时仍滞留在本模块里的 Job，避免内存泄漏
    if (jobInService)
        delete jobInService;
    while (!waitingQueue.empty()) {
        delete waitingQueue.front();
        waitingQueue.pop();
    }
}
