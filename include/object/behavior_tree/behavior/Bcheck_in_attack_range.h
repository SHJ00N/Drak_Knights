#pragma once

#include "object/behavior_tree/BTNode.h"

class Enemy;

class CheckInAttackRange : public BTNode
{
public:
    // constructor
    CheckInAttackRange(Enemy &enemy, float attackRange);
    // override function
    BTState Evaluate(float dt) override;

private:
    Enemy &m_enemy;

    float m_attackRange;
};