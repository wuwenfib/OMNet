// 题目要求：从 NED 参数读取本机地址，门阵列统一命名为 "out"
simple SimpleRouter {
    parameters:
        int address;           // 本机地址，由 omnetpp.ini 注入
    gates:
        input  in;             // 来自上游的输入门
        output out[];          // 动态门阵列，连接下游节点
}
