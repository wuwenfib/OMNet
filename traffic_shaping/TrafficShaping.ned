network TrafficShaping
{
    submodules:
        gen    : TrafficGen;
        tb     : TokenBucket;
        lb     : LeakyBucket;
        sinkTB : Sink;
        sinkLB : Sink;

    connections:
        gen.outTB --> tb.in;
        gen.outLB --> lb.in;
        tb.out    --> sinkTB.in;
        lb.out    --> sinkLB.in;
}
