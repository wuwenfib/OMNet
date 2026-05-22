// DataPacket.msg — 练习 4
// 字段改成 srcAddr + destAddr，和面试题的 AppPacket 完全一致
packet DataPacket
{
    int srcAddr;   // 发送方地址
    int destAddr;  // 目的地址（Router 要查路由表才能知道该从哪个门发出）
}
