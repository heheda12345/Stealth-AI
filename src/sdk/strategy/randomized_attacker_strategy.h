//
// Created by IcyF on 2018/2/18.
//

#ifndef RANDOMIZED_ATTACKER_STRATEGY_H
#define RANDOMIZED_ATTACKER_STRATEGY_H

#include "strategy.h"
#include <set>

class RandomizedAttackerStrategy : public Strategy
{
public:
    RandomizedAttackerStrategy() : Strategy() { }
    virtual ~RandomizedAttackerStrategy() = default;

    virtual void generateActions(const PlayerSight&, Actions*);
    virtual StrategyType getStrategyType() { return Offensive; }
};

#endif //RANDOMIZED_ATTACKER_STRATEGY_H
