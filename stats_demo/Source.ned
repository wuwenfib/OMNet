// 周期性发包的源节点。完全跟统计无关，是 Filter 的输入而已。
simple Source {
    parameters:
        int    numPackets = default(20);   // 发多少个包后停止
        double interval   = default(1.0);  // 每个包间隔（秒）
    gates:
        output out;
}
