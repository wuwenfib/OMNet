simple Sink
{
    parameters:
        string label = default("Sink");
        @display("i=block/sink");
    gates:
        input in;
}
