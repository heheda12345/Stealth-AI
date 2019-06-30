#ifndef DEFS_H
#define DEFS_H

#include "vec2.h"
#include "literals.h"
#include <vector>
#include <string>

enum ActionType
{
    NoAction,                                   //空动作
    SelectDestination,                          //根据Router的指示开始向某一位置移动
    ContinueMovement,                           //继续已经决定好的移动
    BuyItem,                                    //试图购买物品
    UseItem,                                    //试图使用物品
    SuckAttack,                                 //试图对某个单位使用吸血攻击
    ActionTypeCount
};
enum CommandType
{
    NoCommand,                                  //空指令
    MoveTo,                                     //特定单位或炸弹移动到特定地点
    BombThrown,                                 //特定地点扔出了一个炸弹
    BombExplode,                                //特定玩家的炸弹在特定地点爆炸
    WardPlaced,                                 //特定地点放置了一个守卫
    WardExpired,                                //特定守卫抵达时限而消失
    CorpseExpired,                              //特定尸体抵达时限而消失
    BombNumberChange,                           //特定玩家的炸弹数量改变
    WardNumberChange,                           //特定玩家的守卫数量改变
    GoldChange,                                 //特定玩家金钱数目改变
    HPChange,                                   //特定单位生命值改变
    ScoreChange,                                //特定玩家得分改变
	PlayerAttack,								//特定玩家攻击
    UnitDie,                                    //特定单位死亡
    UnitSpawn,                                  //特定单位在特定地点出生
	Replacement,								//特定玩家取代特定村民重生
    DayNightSwitch,                             //昼夜转换
	NightDaySwitch,								//夜昼转换
    GameSet,                                    //游戏结束，并且报告特定玩家胜利
    CommandTypeCount
};

enum LogicStatus
{
    NotInitialized,
    WaitingPlayerActions,
    CalculatingRound,
    DistributingCommands,
    GameEnded,
    LogicStatusCount
};

const int BombItem = 0;
const int WardItem = 1;

struct Action
{
    ActionType actionType;
    int target_id;                           //购买或使用道具时，0为炸弹，1为守卫
    Vec2 pos;

    Action(ActionType _type = NoAction) : actionType(_type), target_id(-1) { }
};
struct Command
{
    CommandType commandType;
    int unit_id, target_id;
    int delta;
    Vec2 pos, direction;

    Command(CommandType _type = NoCommand) : commandType(_type), delta(0) { }
};

struct PWardInfo
{
    Vec2 pos;
    int life;

    PWardInfo() : life(0) { }
};
struct PBombInfo
{
    Vec2 pos;
    Vec2 velocity;

    PBombInfo() = default;
};
struct PUnitInfo
{
    int id;
    Vec2 pos;
    Vec2 velocity;

    PUnitInfo() : id(-1) { }
};

struct PlayerSight
{
    int round;
    int id;
    Vec2 pos;
    int hp;
    Vec2 velocity;

    int bombCount, wardCount;
    bool canUseBomb, canUseWard;
    bool canSuckAttack;

    int gold, score;
    int continuousKill;

    int placedWardCount;
    std::vector<PWardInfo> placedWard;

    int bombInSightCount;
    std::vector<PBombInfo> bombInSight;

    int unitInSightCount;
    std::vector<PUnitInfo> unitInSight;

    int corpseInSightCount;
    std::vector<Vec2> corpseInSight;

    std::vector<int> scoreBoard;

    PlayerSight() = default;
    ~PlayerSight() = default;
};

#endif  //DEFS_H