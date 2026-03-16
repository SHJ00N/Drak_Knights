#pragma once

#include "object/behavior_tree/BTNode.h"

class Enemy;

class Death : public BTNode
{
public:
    // constructor
    Death(Enemy &enemy);
    // override function
    BTState Evaluate(float dt) override;
private:
    Enemy &m_enemy;
};