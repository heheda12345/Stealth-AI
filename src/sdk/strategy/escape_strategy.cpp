//
// Created by IcyF on 2018/2/18.
//

#include "escape_strategy.h"
#include "../randomizer.h"
#include "../router.h"
#include <cmath>

using namespace std;

void EscapeStrategy::generateActions(const PlayerSight &sight, Actions *actions)
{
    CHECK_DISABLED

    Vec2 best(sight.pos);
    float best_score = 0;
    for (int i = 0; i < tries; ++i)
    {
        Vec2 goal;
        do          //使用Randomizer的AxisRnd，通过变换生成一个均匀分布在(0, 2π)的随机角度
        {
            float rad = (Randomizer::getInstance()->randXAxis()) * PI / MapWidth + PI;
            Vec2 e0(cos(rad), sin(rad));
            goal = sight.pos + e0 * PlayerVelocity;
        } while (Router::getInstance()->Reachable(goal));
        float score = 0;
        for (auto uinfo : sight.unitInSight)
        {
            float dis = (uinfo.pos - sight.pos).length();
            if (dis < EPS)
            {
                score = FINF;
                break;
            }
            score += 1 / dis;
        }
        if (score < best_score)
        {
            best_score = score;
            best = goal;
        }
    }
    actions->emplace(SelectDestination, sight.id, best);
}