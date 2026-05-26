simple LeakyBucket
{
    parameters:
        int    bucketSize = default(50);
        double drainRate  = default(500);
        @display("i=block/queue");
    gates:
        input  in;
        output out;
}
