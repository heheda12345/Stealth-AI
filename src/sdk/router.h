#ifndef ROUTER_H
#define ROUTER_H

#include "defs.h"

#include <vector>
#include <iostream>

class Router
{
public:
    ~Router() = default;

    static Router* getInstance();
    bool Reachable(Vec2 x);
    std::vector<Vec2> Route(Vec2 start, Vec2 end);
    Vec2 availablePosition();

private:
    explicit Router();
    static void setObstacle(std::vector<Vec2>& obs, float ldx, float ldy, float rux, float ruy)
    {
        obs.resize(4);          //LD-LU-RU-RD
        obs[0] = Vec2(ldx, ldy);
        obs[1] = Vec2(ldx, ruy);
        obs[2] = Vec2(rux, ruy);
        obs[3] = Vec2(rux, ldy);
    }
    static bool intersect(Vec2 A, Vec2 B, Vec2 C, Vec2 D)
    {
        return ((C - A) * (C - D)) * ((C - B) * (C - D)) <= EPS
            && ((A - C) * (A - B)) * ((A - D) * (A - B)) <= EPS;
    }

    int ObstacleCount, VertexCount;
    std::vector<std::vector<Vec2> > Obstacles;        //n*4数组
    std::vector<Vec2> Vertexes;
    std::vector<std::vector<float> > Edges;
};

#endif  //ROUTER_H