//
// Created by IcyF on 2018/2/22.
//

#ifndef BLIND_ITEM_STRATEGY_H
#define BLIND_ITEM_STRATEGY_H

#include "strategy.h"

class BlindItemStrategy : public Strategy
{
public:
    BlindItemStrategy() : Strategy(), prevBomb(-1), prevWard(-1) { }
    virtual ~BlindItemStrategy() = default;

    virtual void generateActions(const PlayerSight&, Actions*);
    virtual StrategyType getStrategyType() { return Neutral; }

private:
    int prevBomb;
    int prevWard;
};

#endif //BLIND_ITEM_STRATEGY_H
