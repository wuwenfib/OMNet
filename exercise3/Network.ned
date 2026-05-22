// ============================================================
// Network.ned — 练习 3：连接门阵列
// ============================================================
network RouterNetwork
{
    submodules:
        sender    : Sender;
        router    : Router;
        receiver0 : Receiver;
        receiver1 : Receiver;

    connections:
        // Sender → Router 输入门
        sender.out --> router.in;

        // Router 输出门阵列 → 两个 Receiver
        //
        // 【新概念】连接门阵列里的某一个门：用 out[索引] 语法
        // router.out[0] 指第 0 号输出门，连到 receiver0
        // router.out[1] 指第 1 号输出门，连到 receiver1
        router.out[0] --> receiver0.in;
        router.out[1] --> receiver1.in;
}
