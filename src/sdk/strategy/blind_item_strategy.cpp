//
// Created by IcyF on 2018/2/22.
//

#include "blind_item_strategy.h"
#include "../randomizer.h"

using namespace std;

void BlindItemStrategy::generateActions(const PlayerSight &sight, Actions *actions)
{
    CHECK_DISABLED

    int gold = sight.gold;
    if (gold >= BombPrice)
    {
        bool flag = false;
        if (prevBomb == -1)
            flag = true;
        else if ((sight.round - prevBomb) % BombCD == 0)
            flag = true;
        if (flag)
        {
            prevBomb = sight.round;
            actions->emplace(BuyItem, BombItem);
            actions->emplace(UseItem, BombItem, Randomizer::getInstance()->randPosition());
            gold -= BombPrice;
        }
    }
    if (gold >= WardPrice)
    {
        bool flag = false;
        if (prevWard == -1)
            flag = true;
        else if ((sight.round - prevWard) % WardCD == 0)
            flag = true;
        if (flag)
        {
            prevWard = sight.round;
            actions->emplace(BuyItem, WardItem);
            actions->emplace(UseItem, WardItem, sight.pos);
        }
    }
}