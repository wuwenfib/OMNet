// Sender.ned — 发包节点，每隔一段时间发一个包，交替发往 destIndex 0 和 1
simple Sender
{
    parameters:
        int    srcAddr    = default(1);
        double interval   = default(1.0);  // 发包间隔（秒）
        @display("i=block/source");
    gates:
        output out;   // 单个输出门，连向 Router 的 in
}
