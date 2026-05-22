// DataPacket.msg — 练习 3（比练习 2 多了 destIndex 字段）
packet DataPacket
{
    int srcAddr;    // 发送方地址
    int destIndex;  // 目标输出门索引（0 或 1），Router 用它决定从哪个门转发
                    // 练习 4 里会换成 destAddr + 路由表查找，原理相同
}
