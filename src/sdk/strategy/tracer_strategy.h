//
// Created by IcyF on 2018/2/17.
//

#ifndef TRACER_STRATEGY_H
#define TRACER_STRATEGY_H

#include "strategy.h"

class TracerStrategy : public Strategy
{
public:
    TracerStrategy(int _target = -1, float _distance = 5.0) :
            Strategy(), target(_target), distance(_distance) { }
    virtual ~TracerStrategy() = default;

    virtual void generateActions(const PlayerSight&, Actions*);
    virtual StrategyType getStrategyType() { return Offensive; }

    void setTarget(int _target) { target = _target; }
    void setDistance(int _distance) { distance = _distance; }

private:
    int target;
    float distance;
};

#endif //TRACER_STRATEGY_H
