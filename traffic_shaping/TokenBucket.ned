simple TokenBucket
{
    parameters:
        int    bucketSize = default(50);
        double tokenRate  = default(500);
        @display("i=block/process");
    gates:
        input  in;
        output out;
}
