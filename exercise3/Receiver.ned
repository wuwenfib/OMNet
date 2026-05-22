// Receiver.ned — 接收节点，打印收到的包
simple Receiver
{
    parameters:
        @display("i=block/sink");
    gates:
        input in;   // 单个输入门
}
