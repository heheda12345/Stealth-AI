//
// Created by IcyF on 2018/2/8.
//

#ifndef ACTIONS_H
#define ACTIONS_H

#include "defs.h"

struct Actions
{
    std::vector<Action> actions;
    void emplace(ActionType type, int target_id, Vec2 pos);
	void emplace(ActionType type, int target_id);
	void emplace(ActionType type, Vec2 pos);
};

#endif //ACTIONS_H
