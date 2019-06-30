//
// Created by IcyF on 2018/2/19.
//

#ifndef STEALTH_STRATEGY_H
#define STEALTH_STRATEGY_H

#include "strategy.h"

class StealthStrategy : public Strategy
{
public:
    StealthStrategy(int _hp_lower_bound = 20) : Strategy(), hp_lower_bound(_hp_lower_bound),
                                                status(ssStandby), activeRound(0) { }
    virtual ~StealthStrategy() = default;

    virtual void generateActions(const PlayerSight&, Actions*);
    virtual StrategyType getStrategyType() { return Defensive; }

private:
    enum StealthStatus {ssStandby, ssWalking} status;

    int hp_lower_bound;
    int activeRound;
};

#endif //STEALTH_STRATEGY_H
