#include "Generator.h"
#include <iostream>

Define_Module(Generator);

void Generator::initialize()
{
    interArrivalMean = par("interArrivalMean").doubleValue();
    jobCount = 0;

    WATCH(jobCount);   // 让 Qtenv Inspector 能实时看到 jobCount

    // 自消息当定时器：先排第一次生成
    genTimer = new cMessage("genTimer");
    scheduleAt(simTime() + exponential(interArrivalMean), genTimer);
}

void Generator::handleMessage(cMessage *msg)
{
    // 这里收到的只会是自己排的 genTimer
    jobCount++;

    Job *job = new Job("job");
    job->setJobId(jobCount);
    EV << "[Generator] 生成 Job #" << jobCount << endl;
    send(job, "out");

    // 排下一次生成：再抽一个指数分布的间隔
    scheduleAt(simTime() + exponential(interArrivalMean), genTimer);
}

void Generator::finish()
{
    // 用 std::cout 而非 EV：express 模式下 EV 会被屏蔽，
    // 而统计汇总必须无条件打印出来。
    std::cout << "\n==== Generator 统计 ====" << std::endl;
    std::cout << "  生成 Job 总数 : " << jobCount << std::endl;
}

Generator::~Generator()
{
    // genTimer 始终处于"已调度"状态，cancelAndDelete 同时撤销并释放
    cancelAndDelete(genTimer);
}
