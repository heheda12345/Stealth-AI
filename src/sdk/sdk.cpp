//
// Created by IcyF on 2018/2/15.
//

#include "sdk.h"

using namespace std;

bool SDK::reachable(Vec2 x)
{
    return Router::getInstance()->Reachable(x);
}
vector<Vec2> SDK::routeTo(Vec2 start, Vec2 end)
{
    return Router::getInstance()->Route(start, end);
}
float SDK::distanceTo(Vec2 start, Vec2 end)
{
    auto path = routeTo(start, end);
    if (path.empty())
        return FINF;
    float res = (path[0] - start).length();
    for (int i = 1; i < path.size(); ++i)
        res += (path[i] - path[i - 1]).length();
    return res;
}

float SDK::randX() { return Randomizer::getInstance()->randXAxis(); }
float SDK::randY() { return Randomizer::getInstance()->randYAxis(); }
Vec2 SDK::randPosition() { return Randomizer::getInstance()->randPosition(); }