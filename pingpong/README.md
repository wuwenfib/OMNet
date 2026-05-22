# OMNeT++ 入门示例：乒乓消息

这是一个面向完全新手的 OMNeT++ 最小示例项目。  
两个节点 (nodeA / nodeB) 通过一条链路互相弹消息，像打乒乓球。

---

## 项目结构

```
pingpong/
├── Node.ned       NED 文件：描述 Node 模块的接口（有哪些门）
├── Network.ned    NED 文件：描述整个网络拓扑（两个节点 + 一条链路）
├── Node.h         C++ 头文件：Node 类声明
├── Node.cc        C++ 实现：节点的全部行为逻辑
├── omnetpp.ini    仿真配置：网络名、仿真时长等
└── README.md      本文件
```

---

## 运行逻辑

```
t = 0.0s   nodeA  发出 "ping"
t = 0.1s   nodeB  收到 "ping"，等待 1 秒...
t = 1.1s   nodeB  发出 "pong"
t = 1.2s   nodeA  收到 "pong"，等待 1 秒...
t = 2.2s   nodeA  发出 "ping"
t = 2.3s   nodeB  收到 "ping"，等待 1 秒...
...（循环，直到 t = 10s 仿真结束）
```

链路延迟 100ms，每次等待 1 秒，约每 2.2 秒一个完整来回。

---

## 构建与运行

### 第一步：进入项目目录

```bash
cd ~/OMNet++/pingpong
```

### 第二步：确认 OMNeT++ 环境变量已加载

```bash
# 如果 opp_makemake 不在 PATH 里，先执行：
source ~/OMNet++/omnetpp-6.4.0/setenv
```

### 第三步：生成 Makefile

```bash
opp_makemake -f --deep -o pingpong
```

- `-f`：强制覆盖已有 Makefile  
- `--deep`：递归搜索所有子目录里的源文件  
- `-o pingpong`：指定输出可执行文件名

### 第四步：编译

```bash
make -j$(nproc)
```

编译成功后会生成可执行文件 `./pingpong`。

### 第五步：运行（命令行模式）

```bash
./pingpong -u Cmdenv omnetpp.ini
```

- `-u Cmdenv`：使用命令行界面（不弹图形窗口）  
- `omnetpp.ini`：指定配置文件

### 可选：图形界面运行

```bash
./pingpong omnetpp.ini
```

省略 `-u Cmdenv` 会启动 Qtenv 图形界面，可以暂停、单步执行、查看动画。

---

## 预期输出示例

```
** Event #1  t=0s  ...  nodeA  initialize
【nodeA】初始化完成，已安排自消息，将在 t=0 触发首次发送

** Event #2  t=0s  ...  nodeB  initialize
【nodeB】初始化完成，被动等待 nodeA 的消息...

** Event #3  t=0s  ...  nodeA  handleMessage
【t=0s】【nodeA】定时器触发，准备发消息给对方
【t=0s】【nodeA】已发出消息 "ping"（对方 100ms 后收到）

** Event #4  t=0.1s  ...  nodeB  handleMessage
【t=0.1s】【nodeB】收到消息 "ping"，1 秒后回复
【t=0.1s】【nodeB】已安排回复定时器，将在 t=1.1s 发出回复

** Event #5  t=1.1s  ...  nodeB  handleMessage
【t=1.1s】【nodeB】定时器触发，准备发消息给对方
【t=1.1s】【nodeB】已发出消息 "pong"（对方 100ms 后收到）
...
```

---

## 核心概念解释

| 概念 | 解释 |
|------|------|
| `cMessage` | OMNeT++ 中的消息类，是模块间通信的载体，相当于"数据包" |
| `cSimpleModule` | 所有简单模块的 C++ 基类，提供 initialize/handleMessage 框架 |
| `initialize()` | 仿真启动时调用一次，适合发起第一条消息或初始化状态 |
| `handleMessage()` | 每次收到消息时调用，是模块行为的核心逻辑所在 |
| `self-message（自消息）` | 模块发给自己的消息，充当"定时器"，避免使用阻塞的 sleep() |
| `scheduleAt(t, msg)` | 把消息 msg 安排在仿真时间 t 送回自己，触发 handleMessage |
| `send(msg, "gate$o")` | 从 gate 的输出端把消息发出去，沿连线传向对方节点 |
| `EV << "..."` | 向仿真日志输出信息，类似 cout，但集成在仿真框架里 |
| `simTime()` | 返回当前仿真时间（离散的逻辑时间，不是真实时钟） |
| `inout gate` | 双向门，内部自动拆分为 gate$i（接收）和 gate$o（发送） |

---

## 常见问题

**Q：为什么 initialize() 里不能直接 send()？**  
A：可以 send()，但更安全的做法是用自消息，因为在 initialize() 阶段其他模块可能还没完成初始化。用 scheduleAt(simTime(), msg) 把发送推迟到第一个事件循环，确保所有模块都就绪。

**Q：为什么不能用 sleep(1) 来等待 1 秒？**  
A：OMNeT++ 是离散事件仿真（DES），不存在"真正等待"。sleep() 会冻结整个仿真进程。正确方式是用 scheduleAt() 安排未来的事件，引擎会自动推进仿真时钟到该事件。

**Q：收到的消息为什么必须 delete？**  
A：消息对象在堆上分配（new），OMNeT++ 不自动回收。如果不 delete，每次收消息都会泄漏内存，长时间运行会崩溃。唯一的例外是把消息继续 send() 转发出去——转发后消息的所有权转移给引擎，不能再 delete。
