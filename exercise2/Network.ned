// Network.ned — 练习 2
network PingPongNetwork
{
    submodules:
        nodeA : Node;
        nodeB : Node;
    connections:
        nodeA.gate <--> { delay = 100ms; } <--> nodeB.gate;
}
