//
// Created by IcyF on 2018/2/19.
//

#include "patrol_strategy.h"
#include "../randomizer.h"

#include <cstdlib>

using namespace std;

void PatrolStrategy::generateActions(const PlayerSight &sight, Actions *actions)
{
    CHECK_DISABLED

    if (sight.round == 1)
        currentTarget = (int)(Randomizer::getInstance()->randXAxis()) % 5;
    if (sight.round % 40 != 1)
        return;
    currentTarget = (currentTarget + 1) % 5;
    goal = Vec2(Randomizer::getInstance()->randXAxis() * 4 / MapWidth - 2,
                Randomizer::getInstance()->randYAxis() * 4 / MapHeight - 2);
    switch (currentTarget)
    {
        case 0:
            goal = goal + Vec2(27, 82);
            break;
        case 1:
            goal = goal + Vec2(16, 46);
            break;
        case 2:
            goal = goal + Vec2(23, 7);
            break;
        case 3:
            goal = goal + Vec2(40, 55);
            break;
        case 4:
            goal = goal + Vec2(55, 84);
            break;
        default:
            goal = sight.pos;
    }
    actions->emplace(SelectDestination, sight.id, goal);
}