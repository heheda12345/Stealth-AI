//
// Created by IcyF on 2018/2/18.
//

#include "tracer_strategy.h"
#include "../router.h"

using namespace std;

void TracerStrategy::generateActions(const PlayerSight &sight, Actions *actions)
{
    CHECK_DISABLED

    for (auto uinfo : sight.unitInSight)
        if (uinfo.id == target)
        {
            Vec2 goal = uinfo.pos + uinfo.velocity;
            if (!Router::getInstance()->Reachable(goal))
                goal = uinfo.pos;
            Vec2 direction = goal - sight.pos;
            float dist = direction.length();
            if (dist <= distance)
                break;
            Vec2 e0 = direction / dist;
            if (dist - PlayerVelocity <= distance)
                actions->emplace(SelectDestination, sight.id, sight.pos + e0 * (dist - distance));
            else
                actions->emplace(SelectDestination, sight.id, sight.pos + e0 * PlayerVelocity);
            return;
        }
    actions->emplace(SelectDestination, sight.id, sight.pos);
    return;
}