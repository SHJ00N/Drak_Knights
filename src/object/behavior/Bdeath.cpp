#include "object/behavior_tree/behavior/Bdeath.h"
#include "object/enemy/enemy.h"

Death::Death(Enemy &enemy) : m_enemy(enemy)
{
}

BTState Death::Evaluate(float dt)
{
    if(m_enemy.IsDeath)
    {
        BTState state = BTState::Success;
        return state;
    }

    BTState state = BTState::Failure;
    return state;
}