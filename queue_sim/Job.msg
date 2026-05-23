// 排队系统中流通的"任务"消息。
// 注意：没有 creationTime 字段——直接用 cMessage 自带的
// getCreationTime() 即可知道这个消息对象是何时被 new 出来的。
message Job {
    int jobId;   // 任务编号，便于日志辨认
}
