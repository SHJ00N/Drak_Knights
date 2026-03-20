#include "object/behavior_tree/behavior/Benemy_attack.h"
#include "object/enemy/enemy.h"
#include "object/game_object.h"
#include "object/weapon.h"

EnemyAttack::EnemyAttack(Enemy &enemy) : m_enemy(enemy)
{
}

BTState EnemyAttack::Evaluate(float dt)
{
    auto target = GetNodeData<GameObject*>("target");
    auto weapon = m_enemy.GetWeapon();

    if(m_enemy.Animator3D.IsAnimationFinished())
    {
        m_enemy.IsAttacking = false;
        if(target->ObjectDestroyed)
        {
            RemoveNodeData("target");
            m_enemy.Animator3D.PlayAnimation("Walk");
        }
        else
        {
            m_attackTimer = 0.0f;
        }
    }

    m_attackTimer += dt;
    if(m_attackTimer >= 1.0f)
    {
        weapon->EndAttack();
    }
    else if(m_attackTimer >= 0.6f)
    {
        if(!weapon->IsAttacking())
            weapon->StartAttack();
    }

    state = BTState::Running;
    return state;
}