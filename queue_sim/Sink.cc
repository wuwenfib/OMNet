#include "Sink.h"
#include <iostream>

Define_Module(Sink);

void Sink::initialize()
{
    servedCount    = 0;
    sojournTimeSum = 0;

    WATCH(servedCount);
    WATCH(sojournTimeSum);
}

void Sink::handleMessage(cMessage *msg)
{
    Job *job = check_and_cast<Job*>(msg);
    servedCount++;

    // 停留时间 = 离开系统的此刻 − 该 Job 被创建的时刻
    // = 排队延迟 + 服务时长
    double sojourn = (simTime() - job->getCreationTime()).dbl();
    sojournTimeSum += sojourn;

    EV << "[Sink] 收到完成的 Job #" << job->getJobId()
       << "，停留时间 " << sojourn << "s" << endl;
    delete job;     // 终点站：收下后释放内存
}

void Sink::finish()
{
    double dur = simTime().dbl();

    // 用 std::cout：express 模式下 EV 被屏蔽，统计汇总必须无条件打印。
    std::cout << "\n==== Sink 统计 ====" << std::endl;
    std::cout << "  服务完成总数 : " << servedCount << std::endl;
    std::cout << "  吞吐量       : " << (dur > 0 ? servedCount / dur : 0.0) << " job/s" << std::endl;
    std::cout << "  平均停留时间 : " << (servedCount > 0 ? sojournTimeSum / servedCount : 0.0) << "s" << std::endl;
}
