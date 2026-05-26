packet DataPacket
{
    int    seqNum;    // 包序号
    double sendTime;  // 发送时间戳（秒），用于计算端到端延迟
    int    sizeBytes; // 包大小（字节）
}
