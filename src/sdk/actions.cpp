//
// Created by IcyF on 2018/2/8.
//

#include "actions.h"

void Actions::emplace(ActionType type, int target_id, Vec2 pos)
{
    Action act(type);
    act.target_id = target_id;
    act.pos = pos;
    actions.push_back(act);
}
void Actions::emplace(ActionType type, int target_id)
{
	this->emplace(type, target_id, Vec2());
}
void Actions::emplace(ActionType type, Vec2 pos)
{
	this->emplace(type, -1, pos);
}