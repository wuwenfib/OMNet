// ============================================================
// 路由器模块
//
//   亮点（相对 simple_router）：
//   1) 路由表由 omnetpp.ini 注入，格式 "destAddr:portIdx,destAddr:portIdx,..."
//   2) 未命中时若 defaultPort >= 0，则走默认门，避免无谓丢包
//   3) 通过 @signal + @statistic 上报转发/丢包/TTL 过期次数
//   4) 每跳 TTL-1，<=0 即作为 ttlExpired 丢弃
//
//   gates: 使用 inout 双向门阵列 port[]，连线时用 ++ 动态追加
// ============================================================
simple Router
{
    parameters:
        int    address;                          // 路由器本机地址
        string routingTable = default("");       // "1:0,2:1,3:1" 表示 dest=1 走 port[0]，依此类推
        int    defaultPort  = default(-1);       // 未命中时的默认出口；-1 表示真正丢包

        // —— 信号声明 —— 见 .cc 中 emit() 的调用点
        @signal[forwarded](type=long);
        @signal[droppedNoRoute](type=long);
        @signal[ttlExpired](type=long);

        // —— 把信号挂到统计上，仿真结束自动落进 .sca / .vec ——
        @statistic[forwarded](title="Forwarded packets"; source=forwarded; record=count,vector);
        @statistic[droppedNoRoute](title="Dropped (no route)"; source=droppedNoRoute; record=count);
        @statistic[ttlExpired](title="Dropped (TTL expired)"; source=ttlExpired; record=count);

    gates:
        inout port[];                            // 双向门阵列，跟其他路由器或 host 互联
}
