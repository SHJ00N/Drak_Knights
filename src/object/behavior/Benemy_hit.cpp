#include "object/behavior_tree/behavior/Benemy_hit.h"
#include "object/enemy/enemy.h"

EnemyHit::EnemyHit(Enemy &enemy) : m_enemy(enemy)
{
}

BTState EnemyHit::Evaluate(float dt)
{
    if(m_enemy.IsHit)
    {
        if(m_enemy.Animator3D.IsAnimationFinished())
        {
            m_enemy.IsHit = false;
            m_enemy.Animator3D.PlayAnimation("Walk");
            state = BTState::Failure;
            return state;
        }
        else
        {
            state = BTState::Success;
            return state;
        }
    }

    state = BTState::Failure;
    return state;
}