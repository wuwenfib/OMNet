# OMNeT++ 学习笔记

基于 OMNeT++ 6.4.0 的入门练习项目，从零开始逐步掌握网络仿真开发。

## 环境要求

- Ubuntu 24.04 x86-64
- OMNeT++ 6.4.0（Academic Public License）

## 项目结构

```
├── pingpong/      入门示例：两节点互发消息（乒乓球）
├── exercise1/     练习 1：NED 参数
├── exercise2/     练习 2：自定义消息类型
├── exercise3/     练习 3：门阵列与按索引转发
└── exercise4/     练习 4：std::map 路由表
```

## 各练习说明

### pingpong — 入门示例

两个节点 nodeA / nodeB 通过一条 100ms 延迟的链路互发消息，像打乒乓球。仿真运行 10 秒后自动结束。

涵盖概念：`cSimpleModule`、`initialize()`、`handleMessage()`、`cMessage`、`scheduleAt()`、`send()`、`EV` 日志、`inout` 双向门。

### exercise1 — NED 参数

在 pingpong 基础上，将硬编码的等待时间改为可配置的 NED 参数 `waitTime`，通过 `omnetpp.ini` 分别给 nodeA 和 nodeB 设置不同的值，无需重新编译即可调整行为。

涵盖概念：`parameters {}` 块、`default()`、`par("name").doubleValue()`、ini 文件参数覆盖优先级。

### exercise2 — 自定义消息类型

使用 `.msg` 文件定义携带 `srcAddr` 和 `seqNum` 字段的 `DataPacket` 消息，替代普通 `cMessage`。

涵盖概念：`.msg` 文件语法、`opp_msgc` 自动代码生成、getter/setter、`check_and_cast<T*>()` 安全类型转换。

### exercise3 — 门阵列与按索引转发

三节点拓扑（Sender → Router → Receiver×2），Router 拥有 `out[]` 门阵列，根据消息中的 `destIndex` 字段决定从哪个门转发。

涵盖概念：`output out[]` 动态门阵列、`gateSize()`、`send(msg, "out", idx)` 三参数发送、`++` 连线语法。

### exercise4 — std::map 路由表

在 exercise3 基础上，将"直接使用门索引"替换为"查路由表把目的地址转换成门索引"，并完整实现三路分支逻辑：到达终点 / 转发成功 / 丢包。

涵盖概念：`std::map<int,int>`、`map::find()` 安全查找、`it->second` 取值、内存生命周期管理。

## 构建与运行

每个子目录的操作步骤相同：

```bash
# 1. 加载 OMNeT++ 环境（首次或新终端时执行）
source ~/OMNet++/omnetpp-6.4.0/setenv

# 2. 进入子目录
cd pingpong   # 或 exercise1 / exercise2 / exercise3 / exercise4

# 3. 生成 Makefile（首次或添加新文件后执行）
opp_makemake -f --deep -o <目录名>

# 4. 编译
make -j$(nproc)

# 5. 运行（命令行模式）
./out/gcc-release/<目录名> -u Cmdenv omnetpp.ini
```

## 知识点与面试题对照

| 知识点 | 练习 | 对应面试题 SimpleRouter |
|--------|------|------------------------|
| `par("address").intValue()` | exercise1 | `initialize()` 读本机地址 |
| `.msg` 文件 + getter/setter | exercise2 | `AppPacket` 的 `srcAddr`/`destAddr` |
| `check_and_cast<T*>()` | exercise2 | 收到消息后安全转型 |
| `send(pkt, "out", idx)` | exercise3 | 路由成功时转发 |
| `std::map` + `find()` | exercise4 | 路由表查询 |
| 三路分支（到达/转发/丢包） | exercise4 | `handleMessage()` 核心逻辑 |
