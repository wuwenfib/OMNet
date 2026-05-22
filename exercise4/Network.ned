network RouterNetwork
{
    submodules:
        sender    : Sender;
        router    : Router;
        receiver0 : Receiver;
        receiver1 : Receiver;

    connections:
        sender.out  --> router.in;

        // --------------------------------------------------------
        // 动态门阵列的连线语法：用 ++ 代替 [0]、[1]
        //
        // router.out++ 表示"自动追加一个新门"：
        //   第一次 ++ → 创建 out[0] 并连到 receiver0
        //   第二次 ++ → 创建 out[1] 并连到 receiver1
        // 连完后 gateSize("out") == 2
        //
        // 对比固定大小写法（练习3）：router.out[0] --> ...
        // 动态写法好处：增减 Receiver 数量时不用改 Router.ned
        // --------------------------------------------------------
        router.out++ --> receiver0.in;
        router.out++ --> receiver1.in;
}
