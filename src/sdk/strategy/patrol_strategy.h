//
// Created by IcyF on 2018/2/19.
//

#ifndef PATROL_STRATEGY_H
#define PATROL_STRATEGY_H

#include "strategy.h"

class PatrolStrategy : public Strategy
{
public:
    PatrolStrategy() : Strategy() { }
    virtual ~PatrolStrategy() = default;

    /**
     * 沿着地图逆时针旋转。
     * 每次选择位置的时候会在一个区域内随机，而不会是简单的直行。
     */
    virtual void generateActions(const PlayerSight&, Actions*);
    virtual StrategyType getStrategyType() { return Neutral; }

private:
    //enum PatrolStatus {psStandby, psWalking} status;

    /**
     * currentTarget: 描述当前以哪个点为目标。
     */
    int currentTarget;
    Vec2 goal;
};

#endif //PATROL_STRATEGY_H
