// 最小教学拓扑：
//   src --> filter --> sink
//             │
//             └─ emit forwarded / dropped 信号
//                由 @statistic 自动落到 results/*.sca 和 *.vec
network StatsDemo
{
    submodules:
        src    : Source;
        filter : Filter;
        sink   : Sink;
    connections:
        src.out    --> filter.in;
        filter.out --> sink.in;
}
