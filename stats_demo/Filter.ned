// 教学核心：Filter 收一个包，按概率 dropProb 丢弃，其它转发。
// 转发 / 丢弃两类事件分别 emit 信号，由 @statistic 落盘到结果文件。
simple Filter {
    parameters:
        double dropProb = default(0.3);   // 丢包概率（0=全转发，1=全丢）

        // ─── ① @signal —— 仅声明信号的类型契约，不产生任何输出 ───
        @signal[forwarded](type=long);    // 携带值 = 被转发包的序号
        @signal[dropped](type=long);      // 携带值 = 被丢弃包的序号

        // ─── ② @statistic —— 决定把信号落到哪里、记什么 ───
        // record=count  -> .sca 标量：累计次数（"转发了几个 / 丢了几个"）
        // record=vector -> .vec 向量：每次 emit 一条 (t, value)，可画时间序列
        // 仅改这两行 record，就能多出 sum/mean/min/max/histogram —— 0 行 C++
        @statistic[forwarded](
            title  = "Forwarded packets";
            source = forwarded;
            record = count, sum, mean, min, max, histogram, vector);
        @statistic[dropped](
            title  = "Dropped packets";
            source = dropped;
            record = count, sum, mean, histogram, vector);

    gates:
        input  in;
        output out;
}
