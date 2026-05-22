// 测试用收包模块：接收数据包并打印，确认转发链路打通
simple Receiver {
    parameters:
        int address;        // 本机地址，便于日志辨认
    gates:
        input in;
}
