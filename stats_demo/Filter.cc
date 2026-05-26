#include "Filter.h"

Define_Module(Filter);

void Filter::initialize()
{
    dropProb = par("dropProb").doubleValue();

    // 注册信号名 -> 拿到内部 ID。
    // 名字 "forwarded" / "dropped" 必须和 NED 里 @signal[xxx] 的方括号名一致。
    sigForwarded = registerSignal("forwarded");
    sigDropped   = registerSignal("dropped");
}

void Filter::handleMessage(cMessage *msg)
{
    long seq = msg->getKind();    // 用 kind 当序号，方便 vector 记录看趋势

    if (uniform(0, 1) < dropProb) {
        // ── 丢包分支：先 emit 信号，再释放内存 ──
        emit(sigDropped, seq);
        dropCount++;
        EV << "[Filter] 丢弃 seq=" << seq << endl;
        delete msg;
    }
    else {
        // ── 转发分支：先 emit 信号，再 send ──
        emit(sigForwarded, seq);
        fwdCount++;
        EV << "[Filter] 转发 seq=" << seq << endl;
        send(msg, "out");
    }
}

// ============================================================
// finish() —— 仿真结束时（自然终止 / sim-time-limit 触发）调用一次
// 此处仅用于在控制台直观打印汇总；真正的统计落盘由 @statistic 完成
// ============================================================
void Filter::finish()
{
    long total = fwdCount + dropCount;
    double dropRate = (total > 0) ? (double)dropCount / total : 0.0;

    EV_INFO << "========== Filter 统计汇总 ==========" << endl;
    EV_INFO << "  收到包总数 = " << total      << endl;
    EV_INFO << "  转发       = " << fwdCount   << endl;
    EV_INFO << "  丢弃       = " << dropCount  << endl;
    EV_INFO << "  丢包率     = " << dropRate   << endl;
    EV_INFO << "  详细结果见 results/*.sca / *.vec" << endl;
    EV_INFO << "=====================================" << endl;
}
