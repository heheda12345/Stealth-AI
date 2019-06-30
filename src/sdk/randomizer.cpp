//
// Created by IcyF on 2018/2/9.
//

#include <chrono>
#include "randomizer.h"

using namespace std;

Randomizer* Randomizer::getInstance()
{
    static Randomizer instance;
    return &instance;
}
Randomizer::Randomizer()
{
    auto seed = chrono::system_clock::now().time_since_epoch().count();
    RndCore = new mt19937(seed);
    XAxisRnd = new uniform_real_distribution<float>(0, MapWidth);
    YAxisRnd = new uniform_real_distribution<float>(0, MapHeight);
    RespawnRnd = new uniform_real_distribution<float>(0, RespawnRndMax);
    WanderRnd = new uniform_int_distribution<int>(WanderIntervalLB, WanderIntervalUB);
}
Randomizer::~Randomizer()
{
    delete RndCore;
    delete XAxisRnd;
    delete YAxisRnd;
    delete RespawnRnd;
    delete WanderRnd;
}

float Randomizer::randXAxis()
{
    return (*XAxisRnd)(*RndCore);
}
float Randomizer::randYAxis()
{
    return (*YAxisRnd)(*RndCore);
}
Vec2 Randomizer::randPosition()
{
    return Vec2(randXAxis(), randYAxis());
}
float Randomizer::randSpawnRnd()
{
    return (*RespawnRnd)(*RndCore);
}
int Randomizer::randWaitTime()
{
    return (*WanderRnd)(*RndCore);
}