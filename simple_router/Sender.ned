// 测试用发包模块：按固定间隔发出若干数据包，覆盖三种路由场景
simple Sender {
    parameters:
        int srcAddr = default(1);        // 本机源地址
        double interval = default(1.0);  // 发包间隔（秒）
    gates:
        output out;
}
