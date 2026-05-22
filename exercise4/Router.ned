simple Router
{
    parameters:
        int myAddr = default(5);   // Router 自身地址，destAddr==myAddr 表示包到达终点
        @display("i=block/routing");
    gates:
        input  in;
        output out[];   // 动态大小门阵列，大小由 Network.ned 里的连线数自动决定
}
