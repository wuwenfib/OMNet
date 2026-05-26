// ============================================================
// 主机模块：既是源也是 sink
//   - 定时发包到 destinations 中随机选的一个地址
//   - 收到 dest==本机 的包就计为"已送达"，统计跳数
// ============================================================
simple Host
{
    parameters:
        int    address;                                    // 本机地址
        double interArrivalMean @unit(s) = default(1s);    // 指数分布的平均间隔
        string destinations = default("");                 // 逗号分隔目的地，例 "2,3"；空串表示纯 sink
        int    initialTtl   = default(8);                  // 发包时写入的初始 TTL

        @signal[generated](type=long);
        @signal[delivered](type=long);                      // value = 该包累计 hopCount
        @statistic[generated](source=generated; record=count);
        @statistic[delivered](title="Delivered packets";
                              source=delivered;
                              record=count,sum,mean,vector);  // mean → 平均跳数

    gates:
        inout port;
}
