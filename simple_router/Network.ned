// 题目演示拓扑：
//
//   sender ──> router ──out[0]──> receiver0 (addr 10)
//                   └──out[1]──> receiver1 (addr 20)
//
network SimpleRouterNetwork
{
    submodules:
        sender    : Sender;
        router    : SimpleRouter;
        receiver0 : Receiver;
        receiver1 : Receiver;

    connections:
        sender.out --> router.in;

        // out[] 是动态门阵列，用 ++ 在连线时自动追加门：
        //   第一次 ++ 创建 out[0] 连 receiver0
        //   第二次 ++ 创建 out[1] 连 receiver1
        router.out++ --> receiver0.in;
        router.out++ --> receiver1.in;
}
