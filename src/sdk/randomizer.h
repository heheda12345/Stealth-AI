//
// Created by IcyF on 2018/2/9.
//

#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include <random>
#include "vec2.h"

class Randomizer
{
public:
    static Randomizer* getInstance();
    ~Randomizer();

    float randXAxis();
    float randYAxis();
    Vec2 randPosition();
    float randSpawnRnd();
    int randWaitTime();

private:
    Randomizer();

    std::mt19937 *RndCore;
    std::uniform_real_distribution<float> *XAxisRnd, *YAxisRnd;
    std::uniform_real_distribution<float> *RespawnRnd;
    std::uniform_int_distribution<int> *WanderRnd;
};

#endif //RANDOMIZER_H
