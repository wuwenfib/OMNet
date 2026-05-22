// ============================================================
// Network.ned — 定义整个仿真网络的拓扑
// ============================================================
//
// network 关键字声明这是一个顶层网络。
// 仿真引擎会把 network 当作"根节点"，从它开始运行整个仿真。
// omnetpp.ini 里的 network = PingPongNetwork 告诉引擎用哪个网络。
// ============================================================

// network 关键字：这是顶层网络定义，不需要 simple/module 前缀
network PingPongNetwork
{
    submodules:
        // submodules 段声明网络里有哪些子模块（节点）。
        // 格式：  <实例名> : <模块类型>;
        //
        // nodeA 和 nodeB 都是 Node 类型的实例，
        // 对应同一个 C++ 类，但在仿真里是独立运行的两个对象。
        nodeA : Node;   // 节点 A — 负责发起第一条消息
        nodeB : Node;   // 节点 B — 被动等待，收到后回复

    connections:
        // connections 段声明模块之间怎么连线。
        //
        // <--> 表示双向连接（两端都能发也能收）。
        // { delay = 100ms; } 是"信道参数"，表示消息在这条链路上
        // 需要 100 毫秒才能从一端传到另一端。
        //
        // nodeA.gate 对应 Node.ned 里声明的 inout gate，
        // OMNeT++ 会自动把 inout 拆成 gate$i 和 gate$o 两个方向使用。
        nodeA.gate <--> { delay = 100ms; } <--> nodeB.gate;
}
