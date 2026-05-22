// Node.ned — 练习 2（与练习 1 相同，保持简单）
simple Node
{
    parameters:
        int myAddr = default(0);      // 本节点地址，用于填入消息的 srcAddr
        double waitTime = default(1.0);
        @display("i=block/routing");
    gates:
        inout gate;
}
