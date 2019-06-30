#include "router.h"
#include "randomizer.h"
#include <algorithm>

#define MAX_POINTS 100

using namespace std;

Router* Router::getInstance()
{
    static Router instance;
    return &instance;
}
Router::Router()
{
    ObstacleCount = 11;
    Obstacles.resize(ObstacleCount);

    setObstacle(Obstacles[0],  0,  0, 21,  7);
    setObstacle(Obstacles[1], 15,  9, 25.5, 17);
    setObstacle(Obstacles[2],  0, 11, 15, 22);
    setObstacle(Obstacles[3],  0, 22, 25.5, 29);
    setObstacle(Obstacles[4], 31,  7, 42, 28);
    setObstacle(Obstacles[5], 42,  0, 60, 52);
    setObstacle(Obstacles[6], 22, 31, 39, 48);
    setObstacle(Obstacles[7],  0, 44, 10, 54);
    setObstacle(Obstacles[8],  0, 54, 23, 90);
    setObstacle(Obstacles[9], 27, 86, 60, 90);
    setObstacle(Obstacles[10],37, 68, 51, 82);

    for (int i = 0; i < ObstacleCount; ++i)
    {
        Vec2 t0 = Obstacles[i][0] + Vec2(-ObsBorder, -ObsBorder);
        Vec2 t1 = Obstacles[i][1] + Vec2(-ObsBorder, ObsBorder);
        Vec2 t2 = Obstacles[i][2] + Vec2(ObsBorder, ObsBorder);
        Vec2 t3 = Obstacles[i][3] + Vec2(ObsBorder, -ObsBorder);
#define CHECK(t) if (t.x >= 0 && t.x <= MapWidth && t.y >= 0 && t.y <= MapHeight) Vertexes.push_back(t);
        CHECK(t0)
        CHECK(t1)
        CHECK(t2)
        CHECK(t3)
#undef CHECK
    }
    VertexCount = Vertexes.size();
    Edges.resize(VertexCount);
    for (int i = 0; i < VertexCount; ++i)
        Edges[i].resize(VertexCount);
    for (int i = 0; i < VertexCount; ++i)
        for (int j = i + 1; j < VertexCount; ++j)
        {
            bool flag = true;
            for (int u = 0; u < ObstacleCount; ++u)
            {
                for (int v = 0; v < 4; ++v)
                    if (intersect(Vertexes[i], Vertexes[j], Obstacles[u][v], Obstacles[u][(v + 1) & 3]))
                    {
                        flag = false;
                        break;
                    }
                if (!flag)
                    break;
            }
            Edges[i][j] = Edges[j][i] = (flag ? (Vertexes[i] - Vertexes[j]).length() : FINF);
        }
}
bool Router::Reachable(Vec2 pos)
{
    if (pos.x < 0 || pos.x > MapWidth || pos.y < 0 || pos.y > MapHeight)
        return false;
    for (int i = 0; i < ObstacleCount; ++i)
        if (pos.x > Obstacles[i][0].x - ObsBorder + EPS
            && pos.x < Obstacles[i][2].x + ObsBorder - EPS
            && pos.y > Obstacles[i][0].y - ObsBorder + EPS
            && pos.y < Obstacles[i][2].y + ObsBorder - EPS)
            return false;
    return true;
}
Vec2 Router::availablePosition()
{
    Vec2 res = Randomizer::getInstance()->randPosition();
    while (!Reachable(res))
        res = Randomizer::getInstance()->randPosition();
    return res;
}
std::vector<Vec2> Router::Route(Vec2 start, Vec2 end)
{
    std::vector<Vec2> res;
    if (!Reachable(start) || !Reachable(end))
        return res;
    if ((end - start).length() < EPS)
    {
        res.push_back(end);
        return res;
    }

    int Points = VertexCount;

    float dis[MAX_POINTS];					//起点到该点的最短距离
	float edges[MAX_POINTS][MAX_POINTS];    //边权
	Vec2 vertex[MAX_POINTS];			    //参与最短路计算的点的坐标
	int prev[MAX_POINTS];					//点的前驱，回溯最短路用
	bool used[MAX_POINTS];                  //点是否已松弛过

    for (int i = 0; i < Points; ++i)
        vertex[i] = Vertexes[i];
    vertex[Points++] = start;
    vertex[Points++] = end;

    for (int i = 0; i < Points; ++i)
    {
        prev[i] = -1;
        used[i] = false;
        dis[i] = (i == VertexCount ? 0 : FINF);
        edges[i][i] = 0;
        for (int j = i + 1; j < Points; ++j)
        {
            if (i < VertexCount && j < VertexCount)
            {
                edges[i][j] = edges[j][i] = Edges[i][j];
                continue;
            }
            bool flag = true;
            for (int u = 0; u < ObstacleCount; ++u)
            {
                for (int v = 0; v < 4; ++v)
                    if (intersect(vertex[i], vertex[j], Obstacles[u][v], Obstacles[u][(v + 1) & 3]))
                    {
						flag = false;
                        break;
                    }
                if (!flag)
                    break;
            }
            edges[i][j] = edges[j][i] = (flag ? (vertex[i] - vertex[j]).length() : FINF);
        }
    }

    //Dijkstra
    while (true)
    {
        int v = -1;
        for (int i = 0; i < Points; ++i)
            if (!used[i] && (v == -1 || dis[i] < dis[v]))
                v = i;
		if (v == -1)
            break;
        used[v] = true;
        for (int i = 0; i < Points; ++i)
            if (dis[i] > dis[v] + edges[v][i])
            {
                dis[i] = dis[v] + edges[v][i];
                prev[i] = v;
            }
    }

	for (int i = Points - 1; i != Points - 2; i = prev[i])
        res.push_back(vertex[i]);
    reverse(res.begin(), res.end());
    return res;
}