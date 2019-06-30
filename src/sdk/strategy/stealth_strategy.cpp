//
// Created by IcyF on 2018/2/19.
//

#include "stealth_strategy.h"
#include "../router.h"
#include "../randomizer.h"

using namespace std;

void StealthStrategy::generateActions(const PlayerSight &sight, Actions *actions)
{
    CHECK_DISABLED

    if (sight.hp <= hp_lower_bound && sight.canSuckAttack && sight.unitInSightCount > 0)
    {
        PUnitInfo nearest, choice;
        nearest.pos = Vec2(FINF, FINF);
        nearest.velocity = Vec2(FINF, FINF);
        bool found = false;
        for (auto uinfo : sight.unitInSight) {
            if ((nearest.pos - sight.pos).len2() > (uinfo.pos - sight.pos).len2())
                nearest = uinfo;
            if ((uinfo.pos - sight.pos).length() <= SuckRange) {
                if (!found) {
                    found = true;
                    choice = uinfo;
                } else if ((choice.pos - sight.pos).len2() < (uinfo.pos - sight.pos).len2())
                    choice = uinfo;
            }
        }

        if (sight.unitInSightCount != 0)
        {
            if (!found)
                actions->emplace(SelectDestination, sight.id, nearest.pos + nearest.velocity);
            else
            {
                actions->emplace(SuckAttack, choice.id, choice.pos);
                actions->emplace(SelectDestination, sight.id, sight.pos - (choice.velocity * 10.));
            }
            return;
        }
    }
    if (sight.velocity == Vec2())
    {
        if (status == ssWalking)    //开始等待
        {
            status = ssStandby;
            activeRound = sight.round + Randomizer::getInstance()->randWaitTime();
        }
        else if (status == ssStandby && sight.round >= activeRound)
        {
            status = ssWalking;
            activeRound = 0;
            actions->emplace(SelectDestination, sight.id, Router::getInstance()->availablePosition());
        }
    }
}