// ============================================================
// 多级路由网络
//
//   拓扑：
//
//                 h1 (addr=1)            h3 (addr=3)
//                  │                       │
//                 port[0]                 port[1]
//                  │                       │
//                 r1 ────── r2 ────── r3
//                  (10)   port[1] port[1]    (30)
//                          │
//                       port[2]
//                          │
//                         h2 (addr=2)
//
//   连线产生的 port 索引（按 ++ 顺序）：
//     r1.port[0] <-> h1
//     r1.port[1] <-> r2.port[0]
//     r2.port[1] <-> r3.port[0]
//     r2.port[2] <-> h2
//     r3.port[1] <-> h3
//
//   据此推出路由表 → 见 omnetpp.ini
// ============================================================
network RouterNetwork
{
    submodules:
        h1: Host;
        h2: Host;
        h3: Host;
        r1: Router;
        r2: Router;
        r3: Router;

    connections:
        // 主机接入边路由 —— 必须先连，确保 port[0] 是 host 接入口，路由表更易理解
        h1.port  <--> r1.port++;       // r1.port[0]
        r1.port++ <--> r2.port++;      // r1.port[1] ↔ r2.port[0]
        r2.port++ <--> r3.port++;      // r2.port[1] ↔ r3.port[0]
        r2.port++ <--> h2.port;        // r2.port[2]
        r3.port++ <--> h3.port;        // r3.port[1]
}
