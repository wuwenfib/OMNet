simple Sender
{
    parameters:
        int    srcAddr  = default(1);
        double interval = default(1.0);
        @display("i=block/source");
    gates:
        output out;
}
