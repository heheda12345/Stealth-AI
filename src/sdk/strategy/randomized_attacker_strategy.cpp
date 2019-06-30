//
// Created by IcyF on 2018/2/18.
//

#include "randomized_attacker_strategy.h"
#include <algorithm>
#include <vector>

using namespace std;

void RandomizedAttackerStrategy::generateActions(const PlayerSight &sight, Actions *actions)
{
    CHECK_DISABLED

    if (!sight.canSuckAttack)
        return;
    vector<int> targets;
    for (int i = 0; i < sight.unitInSightCount; ++i)
        if ((sight.unitInSight[i].pos - sight.pos).length() <= SuckRange)
            targets.push_back(i);
    random_shuffle(targets.begin(), targets.end());
	if (targets.size() > 0)
		actions->emplace(SuckAttack, sight.unitInSight[targets[0]].id, Vec2());
}