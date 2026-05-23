// 排队系统拓扑：
//
//   generator ──> queueServer ──> sink
//
network QueueNetwork
{
    submodules:
        generator   : Generator;
        queueServer : QueueServer;
        sink        : Sink;

    connections:
        generator.out   --> queueServer.in;
        queueServer.out --> sink.in;
}
