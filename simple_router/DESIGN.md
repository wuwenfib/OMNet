# SimpleRouter 路由模块 — 复试汇报材料

> 本文档供 PPT 生成使用：每个一级/二级标题对应一页或一组幻灯片，
> 正文中的要点可直接作为 bullet，代码块可作为代码展示页。
> 文末附「逐页 PPT 大纲」，可直接照此排版。

---

## 一、题目背景与任务

### 背景
在一个基于 OMNeT++ 构建的有线网络仿真项目中，需要实现一个核心的路由器模块 **SimpleRouter**。网络中流通的数据包使用自定义消息类型 **AppPacket**，包含两个整型字段：源地址 `srcAddr` 与目的地址 `destAddr`。每个路由器启动时被分配一个本机地址，并维护一张静态路由表。

### 任务要求
用 C++ 编写 SimpleRouter 模块的完整实现，使其成为可稳定运行的 OMNeT++ 模块。具体分三部分：

| 要求 | 内容 |
|------|------|
| 类定义与注册 | 继承合适的 OMNeT++ 基类，完成模块注册宏调用 |
| 初始化逻辑 | 读取 NED 参数 `address` 存为成员变量；硬编码一张路由表 |
| 消息处理逻辑 | 收包后安全转型为 AppPacket，实现到达 / 转发 / 丢包三路分支 |

---

## 二、整体方案

### 演示拓扑
```
sender ──> router ──out[0]──> receiver0 (地址 10)
                 └──out[1]──> receiver1 (地址 20)
```
- `sender`：测试发包模块，按固定间隔发出数据包
- `router`：本题核心，SimpleRouter，本机地址 = 5
- `receiver0 / receiver1`：终端收包模块

### 模块清单
| 文件 | 作用 |
|------|------|
| `AppPacket.msg` | 自定义消息类型，定义 srcAddr / destAddr |
| `SimpleRouter.ned/.h/.cc` | **核心**：路由器模块 |
| `Sender.ned/.h/.cc` | 测试发包模块 |
| `Receiver.ned/.h/.cc` | 测试收包模块 |
| `Network.ned` | 网络拓扑定义 |
| `omnetpp.ini` | 仿真参数配置 |

### 数据流
1. Sender 每隔 1 秒构造一个 AppPacket，填入 srcAddr 与 destAddr，发往 router
2. Router 收包 → 判断 destAddr → 到达 / 查表转发 / 丢包
3. 转发成功的包经 out[idx] 抵达对应 Receiver

---

## 三、开发思路：渐进式学习路径

> 解题前，先用 4 个递进练习把 OMNeT++ 的 4 个关键能力逐一吃透，
> 每个练习正好对应 SimpleRouter 的一块知识点。这样最终解题水到渠成。

| 阶段 | 练习 | 掌握的能力 | 对应 SimpleRouter 的部分 |
|------|------|-----------|-------------------------|
| 入门 | pingpong | 模块、消息、`scheduleAt`、`send`、`handleMessage` 生命周期 | 整个模块骨架 |
| 练习 1 | NED 参数 | `parameters{}`、`default()`、`par("x").intValue()`、ini 覆盖 | `initialize()` 读 `address` |
| 练习 2 | 自定义消息 | `.msg` 文件、`opp_msgc` 代码生成、getter/setter、`check_and_cast` | AppPacket 的 srcAddr/destAddr + 安全转型 |
| 练习 3 | 门阵列 | `output out[]`、`gateSize()`、`send(msg,"out",idx)` 三参数发送 | 按门索引转发 |
| 练习 4 | std::map 路由表 | `std::map`、`find()` 安全查找、三路分支、内存生命周期 | `handleMessage()` 核心逻辑 |

**思路总结**：把一道综合题拆成 4 个最小知识单元，逐个验证可运行后再组合。
这样做的好处是——每一步都能独立编译运行、定位问题成本低，最终解题时不再有未知点。

---

## 四、实现机制

### 4.1 自定义消息 AppPacket

`AppPacket.msg`：
```
packet AppPacket {
    int srcAddr;   // 源地址
    int destAddr;  // 目的地址
}
```
- 构建时 `opp_msgc` 自动生成 `AppPacket_m.h / AppPacket_m.cc`
- 自动得到 `getSrcAddr()/setSrcAddr()`、`getDestAddr()/setDestAddr()`
- `packet` 关键字使其继承自 `cPacket`，自带长度、时间戳等网络语义

### 4.2 类定义与注册

`SimpleRouter.h`：
```cpp
class SimpleRouter : public cSimpleModule {
  private:
    int myAddr;                       // 本机地址
    std::map<int, int> routingTable;  // 目的地址 -> 输出门索引
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};
```
- 继承 `cSimpleModule`——OMNeT++ 中"有行为的最小模块"基类
- `SimpleRouter.cc` 中调用 `Define_Module(SimpleRouter);`
  把 C++ 类与 NED 里的 `simple SimpleRouter` 绑定，内核建网时才能实例化

### 4.3 初始化逻辑 initialize()

```cpp
void SimpleRouter::initialize() {
    myAddr = par("address").intValue();   // 从 NED 参数读本机地址
    routingTable[10] = 0;                 // 目的地址 10 -> out[0]
    routingTable[20] = 1;                 // 目的地址 20 -> out[1]
}
```
- `par("address")` 取 NED 参数，参数值由 `omnetpp.ini` 注入（`*.router.address = 5`）
- 路由表用 `std::map<int,int>` 硬编码，key=目的地址，value=输出门索引

### 4.4 消息处理逻辑 handleMessage() —— 三路分支

```cpp
void SimpleRouter::handleMessage(cMessage *msg) {
    AppPacket *pkt = check_and_cast<AppPacket *>(msg);   // 安全转型
    int dest = pkt->getDestAddr();

    if (dest == myAddr) {                    // 分支一：到达终点
        EV << "[到达] ...";
        delete pkt;
    }
    else {
        auto it = routingTable.find(dest);
        if (it != routingTable.end()) {      // 分支二：路由成功，转发
            send(pkt, "out", it->second);
        } else {                             // 分支三：路由失败，丢包
            EV << "[丢包] ...";
            delete pkt;
        }
    }
}
```

| 分支 | 条件 | 动作 | 内存处理 |
|------|------|------|---------|
| 到达 | `destAddr == myAddr` | 打印到达日志 | `delete pkt` |
| 转发 | 路由表命中 | `send(pkt,"out",idx)` | 所有权移交内核，**不能** delete |
| 丢包 | 路由表未命中 | 打印丢包日志 | `delete pkt` |

---

## 五、关键设计点（亮点）

1. **安全转型 `check_and_cast`**
   比 `dynamic_cast` 更安全：类型不符时直接报错终止仿真，而非返回 nullptr 导致后续空指针崩溃。

2. **`map::find()` 而非 `map[]`**
   `operator[]` 查不到 key 时会**静默插入**一个默认条目，污染路由表；`find()` 只查不改，配合 `!= end()` 判断是否命中。

3. **内存生命周期管理**
   OMNeT++ 消息为手动管理。三路分支中：到达和丢包必须 `delete`；转发后所有权交给内核，**绝不能**再 delete，否则 double-free。这是 OMNeT++ 最常见的 bug 来源。

4. **动态门阵列 `out[]`**
   路由器输出门数量不写死，`Network.ned` 用 `router.out++` 在连线时自动追加门。增减下游节点无需改 SimpleRouter 代码。

5. **参数外置到 ini**
   本机地址不硬编码在 C++ 里，而是 NED 参数 + ini 注入。换地址无需重新编译。

---

## 六、演示与验证

### 运行命令
```bash
opp_makemake -f --deep -o simple_router
make -j$(nproc)
./out/gcc-release/simple_router -u Cmdenv omnetpp.ini
```

### 运行结果（关键日志）
```
[SimpleRouter] 启动完成，本机地址 = 5，路由表 = {10->out[0], 20->out[1]}
[Sender] 发出数据包 #0  srcAddr=1  destAddr=5
[到达] 收到来自 1 的数据包，目的地址 5 == 本机地址，已抵达终点
[Sender] 发出数据包 #1  srcAddr=1  destAddr=10
[转发] 数据包目的地址 10 命中路由表，从 out[0] 转发
[Receiver-10] 收到数据包  srcAddr=1  destAddr=10
[Sender] 发出数据包 #2  srcAddr=1  destAddr=20
[转发] 数据包目的地址 20 命中路由表，从 out[1] 转发
[Receiver-20] 收到数据包  srcAddr=1  destAddr=20
[Sender] 发出数据包 #3  srcAddr=1  destAddr=99
[丢包] 数据包目的地址 99 在路由表中无对应条目，丢弃
```

### 测试覆盖
Sender 刻意发出 4 个目的地址，正好覆盖全部三条分支：

| 数据包 | destAddr | 触发分支 | 结果 |
|--------|----------|---------|------|
| #0 | 5 | 到达（=本机地址） | 路由器收下 |
| #1 | 10 | 转发 | → receiver0 |
| #2 | 20 | 转发 | → receiver1 |
| #3 | 99 | 丢包（无路由） | 丢弃 |

---

## 七、总结与可优化方向

### 已完成
- SimpleRouter 完整可运行，三路分支全部验证通过
- 参数外置、动态门阵列、安全转型、内存正确管理

### 可优化方向（体现进一步思考）
- **动态路由表**：路由表也改由 NED 参数 / 配置文件注入，而非硬编码
- **默认路由**：未命中时不直接丢包，转发到一个默认门
- **统计采集**：用 OMNeT++ 的 `signal` + `@statistic` 统计转发数 / 丢包数
- **TTL 机制**：AppPacket 加 TTL 字段，防止环路无限转发
- **多级路由器**：拓展为多跳网络，验证路由表协同

---

## 逐页 PPT 大纲

| 页 | 标题 | 要点 |
|----|------|------|
| 1 | 封面 | OMNeT++ SimpleRouter 路由模块实现 / 姓名 / 日期 |
| 2 | 题目背景与任务 | 有线网络仿真 / AppPacket / 三部分要求表格 |
| 3 | 整体方案 | 拓扑图 / 模块清单 / 数据流 3 步 |
| 4 | 开发思路：渐进式学习路径 | 4 个练习递进表 / "拆成最小知识单元"思想 |
| 5 | 实现机制 ①：消息与类定义 | AppPacket.msg / 继承 cSimpleModule / Define_Module |
| 6 | 实现机制 ②：初始化逻辑 | par("address") 读参数 / std::map 路由表 |
| 7 | 实现机制 ③：三路分支 | handleMessage 代码 / 三分支表格 |
| 8 | 关键设计点 | check_and_cast / find / 内存管理 / 动态门阵列 / 参数外置 |
| 9 | 演示与验证 | 运行日志 / 测试覆盖表（4 包覆盖 3 分支） |
| 10 | 总结与可优化方向 | 已完成 / 5 个优化方向 |

> 建议演示节奏：第 4 页讲"我是怎么学过来的"（渐进路径），
> 第 5-7 页讲"代码怎么实现的"，第 9 页现场跑一遍 Cmdenv 看日志。
