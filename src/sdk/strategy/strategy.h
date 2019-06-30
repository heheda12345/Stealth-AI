//
// Created by IcyF on 2018/2/17.
//

#ifndef STRATEGY_H
#define STRATEGY_H

#include "../actions.h"

#define CHECK_DISABLED  if (this->disabled) return;

enum StrategyType
{
    Defensive,
    Neutral,
    Offensive
};

class ActionMaker;
class Strategy
{
    friend class ActionMaker;
public:
    Strategy() { magic = MAGIC; disabled = false; }
    virtual ~Strategy() = default;

    virtual void generateActions(const PlayerSight&, Actions*) = 0;
    virtual StrategyType getStrategyType() { return Neutral; }

    void Disable() { disabled = true; }
    void Enable() { disabled = false; }

protected:
    unsigned int magic;
    bool disabled;
};

#endif //STRATEGY_H
