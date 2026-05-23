// 有限队列 + 单服务器。整个排队系统的核心。
//   - 空闲时来包 -> 立即服务
//   - 忙且队列未满 -> 入队等待
//   - 忙且队列已满 -> 丢弃
simple QueueServer {
    parameters:
        double serviceTimeMean = default(0.8);  // 平均服务时长（秒）
        int    queueCapacity   = default(5);    // 等待队列容量上限
    gates:
        input  in;
        output out;
}
