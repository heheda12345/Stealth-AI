#include<iostream>
#include<fstream>
#include<algorithm>
#include<cmath>
#include<vector>
#include<ctime>
#include "sdk/sdk.h"

#define CHECK_MY_DISABLED(a) if (this->disabled) return a;
using namespace std;

namespace ZC_Strategy
{
class Unit;
class Bomb;
const Vec2 motionless(0,0);
PlayerSight preSight,curSight;

// choose one and only one of the below definition
//ostream& myLog=cout;
ofstream myLog("log.cpp");


class Unit: public PUnitInfo
{
public:
    Unit(const PUnitInfo& unit); //use when constructing a new Unit
    Unit(const PUnitInfo& unit,const Unit& pre); // use when updating an exist unit

    /*
    corpse: It killed a unit.
    bomb: It threw a bomb.
    attackme: It attacked me.
    strangeVelocity: It didn't move as a villager.
    */
    enum SetPlayerReason { corpse, bomb, attackme, strangeVelocity, size};
    string reasonName[size];

    void update(const PUnitInfo&); // update the unit's information using its information in this round

    void setPlayer(SetPlayerReason a); // set it as a player, the reason is a
    void setAttacked()
    {
        m_attacked=1;
    }

    bool isPlayer() const
    {
        return m_isPlayer;
    }
    bool attacked() const
    {
        return m_attacked;
    }
    bool valid() const
    {
        return m_valid;
    }

    bool operator == (const Unit& unit)
    {
        return this->id==unit.id;
    }
    bool operator < (const Unit &unit)
    {
        return this->id<unit.id;
    }
    friend ostream &operator << (ostream &os, const Unit& unit);

protected:
    //strange route
    void checkRoute(const PUnitInfo&);
    //throw bomb
    void checkBomb(const PUnitInfo&);
    //attack someone
    void checkAttack(const PUnitInfo&);

    void updateVStatus(const PUnitInfo& unit, const Unit& pre);

    bool m_isPlayer,m_attacked,m_valid;
    int m_vStatus; // 0 moving || positive number: stopped rounds
    vector<Vec2> m_trace;
};

class Bomb : public PBombInfo
{
public:
    Bomb(const PBombInfo& bomb); // construct with bomb of this round
    Bomb(const Vec2 &source_, const Vec2 &target_); //set friendly as 1
    friend Bomb newRound(Bomb);
    // return the status of this bomb in the next round
    //change source and moveRound if the bomb become certain
    void setCertain(const Vec2 &target);

    bool certain() const
    {
        return possibleTarget.size();
    }
    bool canUpdate() const
    {
        return moveRound!=3;
    }
    bool isFriendly() const { return friendly; }
    bool moveRounds() const { return moveRound; }

    bool operator == (const PBombInfo &bomb) //bomb is in the next round
    {
        return this->pos+this->velocity==bomb.pos;
    }

    friend ostream &operator << (ostream &os, const Bomb& bomb);

    vector<Vec2> possibleTargets() const { return possibleTarget;}

protected:
    void findPossibleTarget(); // calculated only when uncertain

    vector<Vec2> possibleTarget;
    // the source if certain
    // the source assume that the bomb is detected at its first round
    Vec2 source;
    // move round if certain
    // the time from detected if uncertain
    int moveRound;
    bool friendly;
};

Bomb newRound(Bomb);

class MyStatus
{
public:
    MyStatus(): expose(0), attackCD(0), bombCD(0) {} // unsure
    int attackCD,bombCD;
    bool expose;
    void update();
    void revive();
    void setExpose()
    {
        expose=1;
    };
} myStatus;

namespace SituationSaver
{
vector<PlayerSight> sights;

bool dead(int roundID=curSight.round);
bool insight(const Vec2 &p,int roundID=curSight.round);
int IDofPosition(const Vec2 &p,int roundID); // myself:0 unit:ID blank:-1
Vec2 getTarget(const Vec2 &p, const Vec2 &v)
{
    return SDK::reachable(p+v)?p+v:p;
}

void update(const PlayerSight& sight);
}


namespace UnitSolver
{
vector<Unit> units;

// the position of unit id in vector units
// return -1 if not exist
// may have error if two units are in the same position
int unitsFind(int id);

// merge unit info of this round and last round, detect player using velocity
void update();

// set unit id into a player
void setPlayer(int id,Unit::SetPlayerReason);
}

namespace BombSolver
{
vector<Bomb> bombs;
vector<Bomb> preBombs;

//the index in bombs, return -1 if not exist
int bombFind(const PBombInfo &bomb);
bool inBombRange(const Vec2 &pos);

//merge bomb info of this round and last round, detect player using the source of bomb
void update();
}

namespace CorpseSolver
{
vector<Vec2> newCorpses; //corpse produced in the last round
vector<Vec2> corpses;

//index of corpse p in corpses, return -1 if not exist
int findCorpse(const Vec2 &p);

//detect Player using newCorpses
void detectPlayer(const Vec2 &corpse);

//update corpses & newCorpses, detect Player using function detectPlayer
void update();
}

namespace AttackedSolver //find player if I am attacked
{
void update();
}


//---------------------------------strategy------------------------------------------
// attack units in sight randomly, guarantee that attack one at most one time
class RandomAttackStrategy;
// attack players using bomb
class BombAttackStrategy;
// attack players using suck
class SuckAttackStrategy;
// move like a villager
class VillagerMoveStrategy;
// move to a player
class ToPlayerMoveStrategy;
// move to escape from bombs
class BombEscapeMoveStrategy;
// buy a bomb
class BombBuyStrategy;
// attack villager to make fortune
class ProfitAttackStrategy;

namespace UnitSolver
{
int unitsFind(int id)
{
    for (int i=0; i<units.size(); i++)
        if (units[i].id==id)
            return i;
    return -1;
}

void update()
{
    vector<Unit> newUnit;
    for (const PUnitInfo &unit: curSight.unitInSight)
    {
        int index=unitsFind(unit.id);
        if (index==-1)
            newUnit.push_back(Unit(unit));
        else
            newUnit.push_back(Unit(unit,units[index]));
    }
    units=newUnit;
    for (const Unit & unit:units)
        myLog<<"unit: " <<unit <<endl;
}

void setPlayer(int id,Unit::SetPlayerReason reason)
{
    int index=unitsFind(id);
    if (index==-1)
        return;
    units[index].setPlayer(reason);
}

}

namespace BombSolver
{
int bombFind(const PBombInfo &bomb)
{
    for (int i=0; i<bombs.size(); i++)
        if (bombs[i]==bomb)
            return i;
    return -1;
}

bool inBombRange(const Vec2 &pos,bool round) // last round:0 current round:1
{
    vector<Bomb>& t_bombs=round?bombs:preBombs;
    for (const Bomb& bomb: t_bombs)
    {
    	vector<Vec2> targets=bomb.possibleTargets();
        for (const Vec2& blast : targets)
            if ((pos-blast).length()<BombRadius+EPS)
                return 1;
    }
    return 0;
}

void addMyBomb(const Vec2 &source, const Vec2 &target)
{
    bombs.push_back(Bomb(source,target));
}

void update()
{
    vector<Bomb> newBomb;
    for (const PBombInfo &bomb: curSight.bombInSight)
    {
        if (bombFind(bomb)==-1)
            newBomb.push_back(Bomb(bomb));
    }
    for (const Bomb &bomb: bombs)
    {
        if (bomb.canUpdate())
            newBomb.push_back(newRound(bomb));
    }
    preBombs=bombs;
    bombs=newBomb;
    //for (const Bomb& bomb: bombs)
    //myLog<<bomb<<endl;
}
}

namespace CorpseSolver
{
int findCorpse(const Vec2 &p)
{
    for (int i=0; i<corpses.size(); i++)
        if (corpses[i]==p)
            return i;
    return -1;
}

void detectPlayer(const Vec2 &corpse)
{
    vector<PUnitInfo> attacker;
    for (const Unit &unit: preSight.unitInSight)
    {
        if ((unit.pos-corpse).length()<SuckRange+EPS &&
                UnitSolver::unitsFind(unit.id)!=-1)
            attacker.push_back(unit);
    }
    if (attacker.size()==1)
    {
        UnitSolver::setPlayer(attacker[0].id,Unit::corpse);
    }
}

void update()
{
    newCorpses.clear();
    for (const Vec2 &corpse: curSight.corpseInSight)
    {
        if (findCorpse(corpse)!=-1)
            continue;
        if (SituationSaver::insight(corpse,curSight.round)&&
                !SituationSaver::insight(corpse,curSight.round-1))
            continue; // can not find because of sight change
        newCorpses.push_back(corpse);
        //myLog << " newCorpse : " << corpse.x << " " << corpse.y <<endl;
        detectPlayer(corpse);
    }
    corpses=curSight.corpseInSight;
}

}

namespace AttackedSolver
{
void update()
{
    vector<PUnitInfo> suspect;
    if (curSight.round==1)
        return;
    if (preSight.hp-curSight.hp>BurnDamage&&
            preSight.hp-curSight.hp<BombDamage-SuckDamage*SuckDrainCoeff) // attacked
    {
        for (const PUnitInfo &unit:preSight.unitInSight)
        {
            if ((unit.pos-preSight.pos).length()<SuckRange+EPS)
                suspect.push_back(unit);
        }
    }
    if (suspect.size()==1)
    {
        UnitSolver::setPlayer(suspect[0].id,Unit::attackme);
    }
}
}

namespace SituationSaver
{
// dont know whether the function is true
bool dead(int roundID)
{
    if (roundID==0)
        return 1;
    else
        return sights[roundID].hp<=0;
}

bool isDay(int roundID=curSight.round)
{
    ////myLog<<"test is Day " << roundID << " " << ((roundID-1)%(DayTime+NightTime)<DayTime)<<endl;
    return (roundID-1)%(DayTime+NightTime)<DayTime;
}

bool insight(const Vec2 &p,int roundID)
{
    if (roundID==0)
        return 0;
    const PlayerSight &sight=sights[roundID];
    //in the sight of a ward
    for (PWardInfo ward: sight.placedWard)
    {
        if ((p-ward.pos).length()<WardRadius+EPS)
            return 1;
    }
    if (dead(roundID))
        return 0;
    if (isDay(roundID))
        return 1;
    else
        return (p-sight.pos).length()<NightSight+EPS;
}

void update(const PlayerSight& sight)
{
    preSight=curSight;
    curSight=sight;
    if (sights.empty())
        sights.push_back(PlayerSight()); // as roundId starts from 1
    sights.push_back(sight);
    myLog << " round :  " << sight.round <<endl;
    myLog << " dead ? : " << dead(sight.round) << endl;
    myLog << " day ? : " << isDay(sight.round) << endl;
    myLog <<" myPos : " << sight.pos<<endl;
    myLog << "size of sight" <<sights.size()<<endl;
    //myLog<<"scoreBoard :"<<endl;
    //for (int i=0;i<sight.scoreBoard.size();i++)
    //myLog<<sights[sight.round].scoreBoard[i]<<" ";
    //myLog<<endl;
    for (const PUnitInfo & unit: sight.unitInSight)
        myLog<< "unitInSight " << unit.id <<" : "<<unit.pos<<unit.velocity<<endl;
    UnitSolver::update();
    BombSolver::update();
    CorpseSolver::update();
    AttackedSolver::update();
}

int IDofPosition(const Vec2 &p, int roundID)
{
    if (roundID<1)
        return 0;
    for (const PUnitInfo& unit: sights[roundID].unitInSight)
        if (unit.pos==p)
            return unit.id;
    return -1;
}

int alivePlayers(int roundID)
{
    int n=curSight.scoreBoard.size();
    int sum=0;
    for (int i=0; i<n; i++)
        for (int j=roundID; j>1&&j>roundID-RespawnTime; j--)
            sum+=(sights[j].scoreBoard[i]>sights[j-1].scoreBoard[i]);
    //myLog<<"alivePlayers"<<n-sum<<endl;
    return n-sum;
}
}

Unit::Unit(const PUnitInfo& unit):
    PUnitInfo(unit),m_isPlayer(0),m_attacked(0),
    reasonName(
{"corpse","bomb","attackme","velocity"
})
{
    if (unit.velocity==motionless)
    {
        ////myLog<<"constructing new unit"<<SituationSaver::isDay(curSight.round-1)<<SituationSaver::isDay(curSight.round)
        //    <<SituationSaver::dead(curSight.round-1)<<SituationSaver::dead(curSight.round)<<endl;
        if (curSight.round!=1 &&
                ( (!SituationSaver::isDay(curSight.round-1)&&SituationSaver::isDay(curSight.round)) ||
                  (SituationSaver::dead(curSight.round-1)&&!SituationSaver::dead(curSight.round))   ||
                  !SituationSaver::isDay(curSight.round)))
        {
            m_vStatus=-2;
        }
        else
            m_vStatus=15;
    }
    else
        m_vStatus=-1;
    m_trace.push_back(unit.pos);
}

Unit::Unit(const PUnitInfo& unit, const Unit& pre): PUnitInfo(unit)
{
    m_isPlayer=pre.isPlayer();
    m_attacked=pre.attacked();
    m_valid=pre.valid();
    m_vStatus=pre.m_vStatus;
    m_trace=pre.m_trace;
    m_trace.push_back(unit.pos);
    updateVStatus(unit,pre);
    //detect player using route
    auto notEqual=[](float a, float b)
    {
        return abs(a-b)>0.05;
    };
    auto strangeRoute=[notEqual](Vec2 &p1, Vec2 &p2, Vec2 &p3)
    {
        return notEqual(SDK::distanceTo(p1,p2)+SDK::distanceTo(p2,p3),SDK::distanceTo(p1,p3));
    };
    if (!(unit.velocity==motionless))
    {
        Vec2 target=unit.pos+unit.velocity;
        if (m_trace.size()>=2&&SDK::reachable(target)&&m_trace.size()>1
                &&strangeRoute( *(--(--m_trace.end())), *(--m_trace.end()), target))
        {
            Vec2 p1=*(--(--m_trace.end())),p2=*(--m_trace.end()),p3=target;
            myLog<<"strange4"<<SDK::reachable(p1)<<SDK::reachable(p2)<<SDK::reachable(p3)<<p1<<"->"<<p2<<"->"<<p3<<endl;
            myLog<<(SDK::distanceTo(p1,p2))<<"+"<<(SDK::distanceTo(p2,p3))<<"="<<(SDK::distanceTo(p1,p2))+(SDK::distanceTo(p2,p3))<<" "<<(SDK::distanceTo(p1,p3))<<endl;
            setPlayer(Unit::strangeVelocity);
        }
    }
}

void Unit::updateVStatus(const PUnitInfo &unit, const Unit& pre)
{
    if (unit.velocity==motionless)
    {
        if (m_vStatus!=-2)
        {
            m_vStatus++;
            if (m_vStatus>20)
            {
                myLog<<"strange1"<<endl;
                setPlayer(Unit::strangeVelocity);
            }
        }
    }
    else
    {
        if (m_vStatus==-1)
        {
            /*
               auto multiply = [](const Vec2 &a, const Vec2 &b) { return a.x*b.x+a.y*b.y;};
               if (multiply(pre.velocity,unit.velocity)<-EPS) // villager do not turn around
               {
                   //myLog<<pre.velocity.x<<" "<<pre.velocity.y<< " " << unit.velocity.x << " " <<unit.velocity.y<<endl;
                   setPlayer(Unit::strangeVelocity);
               }*/
        }
        else
        {
            if (m_vStatus!=-2&&m_vStatus<10)
            {
                myLog<<"strange3"<<endl;
                setPlayer(Unit::strangeVelocity);
            }
            m_vStatus=-1;
        }
    }
}

ostream &operator << (ostream &os, const Unit& unit)
{
    os << "Unit " << unit.id << " : pos " << unit.pos.x << " " << unit.pos.y << "is player " << unit.isPlayer() << " velocity " <<unit.velocity.x <<" " <<unit.velocity.y << " vstatus "<<unit.m_vStatus;
    return os;
}

void Unit::setPlayer(SetPlayerReason a)
{
    m_isPlayer=1;
    myLog << "!" <<pos.x << " " <<pos.y << " vstatus " << m_vStatus << " velocity " <<velocity <<endl;
    switch (a)
    {
    case corpse :
        myLog << "setPlayer " << id << " reason corpse" <<endl;
        break;
    case bomb :
        myLog << "setPlayer " << id << " reason bomb" <<endl;
        break;
    case attackme :
        myLog << "setPlayer " << id << " reason attackme" <<endl;
        break;
    case strangeVelocity :
        myLog << "setPlayer " << id << " reason strangeVelocity" <<endl;
        break;
    }
}

Bomb::Bomb(const PBombInfo& bomb) : PBombInfo(bomb), friendly(0)
{
    moveRound=-1;
    Vec2 bombPosition=bomb.pos-velocity;
    for (int round=1; round<=3; round++)
    {
        int unitID;
        if ((unitID=SituationSaver::IDofPosition(bombPosition,curSight.round-round))!=-1)
        {
            source=bombPosition;
            moveRound=round;
            UnitSolver::setPlayer(unitID,Unit::bomb);
            break;
        }
        bombPosition=bombPosition-velocity;
    }
    if (moveRound>0) //find source
    {
        possibleTarget.push_back(source+bomb.velocity*3);
    }
    else
    {
        source=bomb.pos-velocity;
        findPossibleTarget();
        moveRound=1;
    }
}

Bomb::Bomb(const Vec2 &source_, const Vec2 &target_)
{
    pos=source_;
    velocity=(target_-source_)*(1.0/3);
    source=source_;
    possibleTarget.push_back(target_);
    friendly=1;
    moveRound=0;
    this->friendly=1;
}

Bomb newRound(Bomb bomb)
{
    bomb.moveRound++;
    if (!bomb.certain())
        bomb.findPossibleTarget();
    return bomb;
}

void Bomb::findPossibleTarget()
{
    Vec2 bombPosition=source+velocity*moveRound;
    for (int i=moveRound; i<=3; i++)
    {
        if (SDK::reachable(bombPosition))
            possibleTarget.push_back(bombPosition);
        bombPosition=bombPosition+velocity;
    }
    if (possibleTarget.size()==1)
        setCertain(possibleTarget[0]);
}

void Bomb::setCertain(const Vec2& target)
{
    source=target-velocity*3;
    moveRound=round(Vec2(pos-source).length()/velocity.length());
}

ostream &operator << (ostream &os, const Bomb& bomb)
{
    os << "Bomb from: " << bomb.source.x << bomb.source.y <<" move Round "<< bomb.moveRound;
    return os;
}

//--------------------------strategy declaration------------------------------
class RandomAttackStrategy : public Strategy
{
public:
    RandomAttackStrategy() : Strategy() { }
    virtual ~RandomAttackStrategy() = default;

    virtual void generateActions(const PlayerSight&, Actions*);

protected:
    vector<int> attacked;
};
RandomAttackStrategy randomAttackStrategy;

class BombAttackStrategy : public Strategy
{
public:
    BombAttackStrategy() : Strategy() { }
    virtual ~BombAttackStrategy() = default;

    virtual bool generateActions(const Vec2 &pos, const PlayerSight&, Actions*);
private:
    void generateActions(const PlayerSight&, Actions*) override {};
};
BombAttackStrategy bombAttackStrategy;

class SuckAttackStrategy : public Strategy
{
public:
    SuckAttackStrategy() : Strategy() { }
    virtual ~SuckAttackStrategy() = default;

    virtual int generateActions(int id, const PlayerSight&, Actions*); // fail 0 move 1 attack 2
private:
    void generateActions(const PlayerSight&, Actions*) override {};
};
SuckAttackStrategy suckAttackStrategy;

class ProfitAttackStrategy : public Strategy
{
public:
    ProfitAttackStrategy() : Strategy(), target(-1) { }
    virtual ~ProfitAttackStrategy() = default;

    virtual void generateActions(const PlayerSight&, Actions*) override;
    int target;
};
ProfitAttackStrategy profitAttackStrategy;

/*
class MoveStrategy : public Stragety
{
public:
    MoveStrategy() : Strategy() { }
    virtual ~MoveStrategy() = default;

    virtual void generateActions(const Vec2 &p, const PlayerSight&, Actions*); // need clear action round
    virtual void preVillagerActions(const PlayerSight&, Actions); //move as villager using target of last round
    virtual void newVillagerActions(const PlayerSight&, Actions);
};
*/
class VillagerMoveStrategy : public Strategy
{
public:
    VillagerMoveStrategy() : Strategy(),actionRound(-1) { }
    virtual ~VillagerMoveStrategy() = default;

    //0 fail 1 go 2 rest
    //use the first parameter only to specify from the pure "generateActions" in base class
    virtual int generateActions(bool,const PlayerSight&, Actions*);
    void generateStartTime();
    void clearActionRound()
    {
        actionRound=-1;
    }
    int actionRound;
private:
    void generateActions(const PlayerSight&, Actions*) override {};
};
VillagerMoveStrategy villagerMoveStrategy;

class ToPlayerMoveStrategy : public Strategy
{
public:
    ToPlayerMoveStrategy() : Strategy() { }
    virtual ~ToPlayerMoveStrategy() = default;

    virtual bool generateActions(int id, const PlayerSight&, Actions*);
private:
    void generateActions(const PlayerSight&, Actions*) override {};
};
ToPlayerMoveStrategy toPlayerMoveStrategy;

class BombEscapeMoveStrategy : public Strategy
{
public:
    BombEscapeMoveStrategy() : Strategy() { }
    virtual ~BombEscapeMoveStrategy() = default;
    // use the first parameter only to specify from the pure "generateActions" in base class
    virtual bool generateActions(bool, const PlayerSight&, Actions*);
private:
    void generateActions(const PlayerSight&, Actions*) override {};
};
BombEscapeMoveStrategy bombEscapeMoveStrategy;

class BombBuyStrategy : public Strategy
{
public:
    BombBuyStrategy() : Strategy() { }
    virtual ~BombBuyStrategy() = default;

    // use the first parameter only to specify from the pure "generateActions" in base class
    virtual bool generateActions(bool,const PlayerSight&, Actions*);
private:
    void generateActions(const PlayerSight&, Actions*) override {};
};
BombBuyStrategy bombBuyStrategy;
//---------------------------generateActions definition-------------------------------------
void RandomAttackStrategy::generateActions(const PlayerSight& sight, Actions* actions)
{
    CHECK_DISABLED

    if (!sight.canSuckAttack)
        return;
    vector<int> targets;
    for (int i = 0; i < sight.unitInSightCount; ++i)
    {
        if (sight.unitInSight[i].id+10>attacked.size())
            attacked.resize(sight.unitInSight[i].id+10);
        if ((sight.unitInSight[i].pos - sight.pos).length() <= SuckRange&&
                !attacked[sight.unitInSight[i].id])//attack each unit at most once
            targets.push_back(i);
    }
    random_shuffle(targets.begin(), targets.end());
    if (targets.size() > 0)
    {
        actions->emplace(SuckAttack, sight.unitInSight[targets[0]].id, Vec2());
        CorpseSolver::corpses.push_back(sight.unitInSight[targets[0]].pos);
        attacked[sight.unitInSight[targets[0]].id]=1;
    }
}

bool BombAttackStrategy::generateActions(const Vec2 &pos, const PlayerSight &sight, Actions* actions)
{
    CHECK_MY_DISABLED(0)

    if (!sight.canUseBomb)
        return 0;
    if (!sight.bombCount&&!bombBuyStrategy.generateActions(0,sight,actions))
        return 0;
    actions->emplace(UseItem,BombItem,pos);
    //myLog<< "throw bomb " << pos.x <<" " <<pos.y<<endl;
    BombSolver::addMyBomb(sight.pos,pos);
    myStatus.setExpose();
    bombBuyStrategy.Disable();
    return 1;
}

int SuckAttackStrategy::generateActions(int id, const PlayerSight &sight, Actions* actions)
{
    CHECK_MY_DISABLED(0)

    if (SituationSaver::dead())
        return 0;
    int index=UnitSolver::unitsFind(id);
    if (index==-1)
        return 0;
    const Unit& unit=UnitSolver::units[index];
    if (sight.canSuckAttack&&(unit.pos-sight.pos).length()<SuckRange+EPS)
    {
        actions->emplace(SuckAttack,id);
        CorpseSolver::corpses.push_back(unit.pos);
        myStatus.setExpose();
        randomAttackStrategy.Disable();
        return 2;
    }
    else
    {
        if (!toPlayerMoveStrategy.generateActions(id,sight,actions))
            return 0;
        else
            return 1;
    }

}

bool ToPlayerMoveStrategy::generateActions(int id, const PlayerSight &sight, Actions* actions)
{
    CHECK_MY_DISABLED(0);

    if (SituationSaver::dead())
        return 0;
    int index=UnitSolver::unitsFind(id);
    if (index==-1)
        return 0;
    const Unit& unit=UnitSolver::units[index];

    double dist=SDK::distanceTo(sight.pos,SituationSaver::getTarget(unit.pos,unit.velocity));
    bool nearDeath=sight.hp<20;
    if (sight.canSuckAttack&&(nearDeath||dist/PlayerVelocity>SuckAttackCD))
        randomAttackStrategy.generateActions(sight,actions);
    actions->emplace(SelectDestination,SituationSaver::getTarget(unit.pos,unit.velocity));

    myStatus.setExpose();
    randomAttackStrategy.Disable();
    villagerMoveStrategy.Disable();
    villagerMoveStrategy.clearActionRound();
    return 1;
}

void VillagerMoveStrategy::generateStartTime()
{
    if (SituationSaver::dead())
        return;
    if (curSight.round==1)
    {
        actionRound=1;
        return;
    }
    int stopRound=curSight.round;
    while (stopRound>1&&SituationSaver::sights[stopRound].pos==SituationSaver::sights[stopRound-1].pos)
        stopRound--;
    actionRound=max(stopRound+Randomizer::getInstance()->randWaitTime(),curSight.round);

}

int VillagerMoveStrategy::generateActions(bool para, const PlayerSight &sight, Actions* actions)
{
    CHECK_MY_DISABLED(0);

    if (actionRound!=-1)
    {
        if (sight.velocity==Vec2())
            actionRound=-1;
        else
        {
            //actions->emplace(ContinueMovement); // do not need
            return 1;
        }
    }
    if (SituationSaver::dead)
        actionRound=-1;
    if (actionRound==-1)
        generateStartTime();
    if (actionRound==sight.round)
    {
        actions->emplace(SelectDestination, sight.id, Router::getInstance()->availablePosition());
        return 1;
    }
    actions->emplace(SelectDestination, sight.id, sight.pos);
    return 2;
}

bool BombEscapeMoveStrategy::generateActions(bool para, const PlayerSight &sight, Actions* actions)
{
    CHECK_MY_DISABLED(0);

    Vec2 danger(-1,-1);
    int remainRound;
    for (const Bomb& bomb: BombSolver::bombs)
    {
        if (bomb.isFriendly())
            continue;
        const vector<Vec2> &targets=bomb.possibleTargets();
        for (const Vec2 &p: targets)
            if ((p-sight.pos).length()<BombRadius+EPS)
            {
                danger=p;
                remainRound=3-bomb.moveRounds();
                break;
            }
    }
    if (danger==Vec2(-1,-1))
        return 0;
    //myLog<<"dangerBomb:"<<danger<<endl;
    double needEscape=BombRadius+EPS-(danger-sight.pos).length();
    if (needEscape>(3-remainRound)*PlayerVelocity)
        return 0;
    Vec2 target=sight.pos+danger*(needEscape*PlayerVelocity/danger.length());
    if (SDK::reachable(target))
    {
        actions->emplace(SelectDestination,target);
        villagerMoveStrategy.clearActionRound();
        toPlayerMoveStrategy.Disable();
        villagerMoveStrategy.Disable();
        myStatus.setExpose();
        return 1;
    }
    return 0;
}

bool BombBuyStrategy::generateActions(bool para,const PlayerSight &sight, Actions* actions)
{
    CHECK_MY_DISABLED(0);

    if (sight.bombCount)
        return 0;
    if (sight.gold>=BombPrice)
    {
        actions->emplace(BuyItem,BombItem);
        return 1;
    }
}

void ProfitAttackStrategy::generateActions(const PlayerSight& sight, Actions* actions)
{
    CHECK_DISABLED;
    if (SituationSaver::dead())
        return;
    for (const Unit& unit: UnitSolver::units)
        if (unit.isPlayer())
            return;
    if (/*SituationSaver::alivePlayers(sight.round)==1||*/!SituationSaver::isDay(sight.round))
    {
        if (target==-1)
        {
            for (const Unit& unit:UnitSolver::units)
                if (unit.attacked())
                {
                    target=unit.id;
                    break;
                }
        }
        if (target==-1)
        {
            float dist=100000;
            for (const Unit& unit:UnitSolver::units)
                if ((unit.pos-sight.pos).length()<dist)
                    target=unit.id,dist=(unit.pos-sight.pos).length();
        }
        if (target==-1)
            return;
        suckAttackStrategy.generateActions(target,sight,actions);
    }
    else
        target=-1;
}

namespace ActionGenerater
{
static int attackID=-1;
const Unit* targetUnit=nullptr;

void commitTargetPlayer(const PlayerSight &sight)
{
    if (attackID!=-1&&UnitSolver::unitsFind(attackID)==-1)
        attackID=-1,targetUnit=nullptr;
    if (attackID==-1)
    {
        for (const Unit& unit: UnitSolver::units)
            if (unit.isPlayer())
            {
                if (targetUnit==nullptr)
                    targetUnit=&unit;
                else
                {
                    if (SDK::distanceTo(sight.pos, SituationSaver::getTarget(unit.pos,unit.velocity)) <
                            SDK::distanceTo(sight.pos, SituationSaver::getTarget(targetUnit->pos,targetUnit->velocity)))
                    {
                        targetUnit=&unit;
                    }
                }
            }
        if (targetUnit!=nullptr)
            attackID=targetUnit->id;
    }
    else
        targetUnit=&UnitSolver::units[UnitSolver::unitsFind(attackID)];

}

void generateActions(const PlayerSight &sight, Actions* actions)
{
    bombEscapeMoveStrategy.generateActions(0,sight,actions);
    profitAttackStrategy.generateActions(sight,actions);

    commitTargetPlayer(sight);
    //myLog<<"attackID" << attackID <<endl;
    if (attackID!=-1)
    {
        bombAttackStrategy.generateActions(targetUnit->pos,sight,actions);
        suckAttackStrategy.generateActions(attackID,sight,actions);
    }

    randomAttackStrategy.generateActions(sight,actions);
    villagerMoveStrategy.generateActions(0,sight,actions);
    bombBuyStrategy.generateActions(0,sight,actions);
}

void enableActions()
{
    randomAttackStrategy.Enable();
    bombAttackStrategy.Enable();
    suckAttackStrategy.Enable();
    villagerMoveStrategy.Enable();
    toPlayerMoveStrategy.Enable();
    bombEscapeMoveStrategy.Enable();
    bombBuyStrategy.Enable();
    profitAttackStrategy.Enable();
}
}
}

extern "C"
{

    AI_API void playerAI(const PlayerSight sight, Actions* actions)
    {
        ZC_Strategy::SituationSaver::update(sight);
        ZC_Strategy::ActionGenerater::generateActions(sight,actions);
        ZC_Strategy::ActionGenerater::enableActions();
    }

}
