#pragma once

#include "object/behavior_tree/BTNode.h"

class Enemy;

class EnemyHit : public BTNode
{
public:
    EnemyHit(Enemy &enemy);
    // override function
    BTState Evaluate(float dt) override;

private:
    Enemy &m_enemy;
};