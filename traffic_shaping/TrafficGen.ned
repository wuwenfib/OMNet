simple TrafficGen
{
    parameters:
        double onTime   @unit(s) = default(0.5s);
        double offTime  @unit(s) = default(0.3s);
        int    sendRate = default(1000);
        int    pktSize  = default(512);
        @display("i=block/source");
    gates:
        output outTB;
        output outLB;
}
