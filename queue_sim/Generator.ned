// 任务生成器：按指数分布的随机间隔不断产生 Job。
// 指数分布的到达间隔 = 泊松到达过程，这正是排队论里的 "M"。
simple Generator {
    parameters:
        double interArrivalMean = default(1.0);  // 平均到达间隔（秒）
    gates:
        output out;
}
