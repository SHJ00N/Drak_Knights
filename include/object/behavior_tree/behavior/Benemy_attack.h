#pragma once

#include "object/behavior_tree/BTNode.h"

class Enemy;
class GameObject;

class EnemyAttack : public BTNode
{
public:
    EnemyAttack(Enemy &enemy);
    // override function
    BTState Evaluate(float dt);
    
private:
    Enemy &m_enemy;

    float m_attackTimer = 0.0f;
};