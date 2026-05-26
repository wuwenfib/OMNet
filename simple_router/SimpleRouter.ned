// 题目要求：从 NED 参数读取本机地址，门阵列统一命名为 "out"
simple SimpleRouter {
    parameters:
        int    address;                       // 本机地址，由 omnetpp.ini 注入
        // 路由表，由 omnetpp.ini 注入。
        // 格式："destAddr:gateIdx" 多条用空格或逗号分隔，例如 "10:0 20:1"
        string routingTable = default("");

        // ─── ① @signal：仅声明信号类型契约，不产生输出 ───
        //   名字必须与 SimpleRouter.cc 里 registerSignal("xxx") 完全一致
        @signal[delivered](type=long);   // value = 该包的 destAddr
        @signal[forwarded](type=long);
        @signal[dropped](type=long);

        // ─── ② @statistic：决定信号怎么落到结果文件 ───
        //   record=count   -> .sca 标量：累计次数
        //   record=vector  -> .vec 时间序列：每次 emit 一条 (t, value)
        //   想多记几样（sum/mean/min/max/histogram）就往 record= 后面加
        @statistic[delivered](
            title  = "Packets delivered to this router";
            source = delivered;
            record = count, vector);
        @statistic[forwarded](
            title  = "Packets forwarded";
            source = forwarded;
            record = count, vector);
        @statistic[dropped](
            title  = "Packets dropped (no route)";
            source = dropped;
            record = count, vector);

    gates:
        input  in;             // 来自上游的输入门
        output out[];          // 动态门阵列，连接下游节点
}
