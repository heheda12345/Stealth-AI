//
// Created by IcyF on 2018/2/18.
//

#ifndef ESCAPE_STRATEGY_H
#define ESCAPE_STRATEGY_H

#include "strategy.h"

class EscapeStrategy : public Strategy
{
public:
    EscapeStrategy(int _tries = 10) : Strategy(), tries(_tries) { }
    virtual ~EscapeStrategy() = default;

    /**
     * 随机选取tries个方向，找一个“离别人最远”的地方。
     * 判断基准是把视野内的所有单位到自己距离的倒数求和。
     */
    virtual void generateActions(const PlayerSight&, Actions*);
    virtual StrategyType getStrategyType() { return Defensive; }

private:
    int tries;
};

#endif //ESCAPE_STRATEGY_H
