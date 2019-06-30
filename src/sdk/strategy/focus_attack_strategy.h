//
// Created by IcyF on 2018/2/19.
//

#ifndef FOCUS_ATTACK_STRATEGY_H
#define FOCUS_ATTACK_STRATEGY_H

#include "strategy.h"
#include "../literals.h"

class FocusAttackStrategy : public Strategy
{
public:
    FocusAttackStrategy() : Strategy(), target(-1) { }
    virtual ~FocusAttackStrategy() = default;

    virtual void generateActions(const PlayerSight&, Actions*);
    virtual StrategyType getStrategyType() { return Offensive; }

    void setTarget(int _target) { target = _target; }

private:
    int target;
};

#endif //FOCUS_ATTACK_STRATEGY_H
