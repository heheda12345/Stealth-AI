//
// Created by IcyF on 2018/2/19.
//

#include "focus_attack_strategy.h"

using namespace std;

void FocusAttackStrategy::generateActions(const PlayerSight &sight, Actions *actions)
{
    CHECK_DISABLED

    bool found = false;
    float dist = FINF;
    int nearest = -1;
    for (auto uinfo : sight.unitInSight)
    {
        if (uinfo.id == target)
        {
            found = true;
            break;
        }
        if ((uinfo.pos - sight.pos).length() < dist)
        {
            dist = (uinfo.pos - sight.pos).length();
            nearest = uinfo.id;
        }
    }
    if (!found)
        target = nearest;
    if (target == -1)
        return;
    
    for (auto uinfo : sight.unitInSight)
    {
        if (uinfo.id != target)
            continue;
        if (sight.canSuckAttack && (uinfo.pos - sight.pos).length() <= SuckRange)
            actions->emplace(SuckAttack, target, uinfo.pos);
        if (sight.canUseBomb)
        {
            if (sight.bombCount == 0)
                actions->emplace(BuyItem, BombItem, Vec2());
            actions->emplace(UseItem, BombItem, uinfo.pos);
        }
    }
}