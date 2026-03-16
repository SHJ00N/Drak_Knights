#include "object/behavior_tree/behavior/Bcheck_in_attack_range.h"
#include "object/enemy/enemy.h"

CheckInAttackRange::CheckInAttackRange(Enemy &enemy, float attackRange) : m_enemy(enemy), m_attackRange(attackRange)
{
}

BTState CheckInAttackRange::Evaluate(float dt)
{
    auto target = GetNodeData<GameObject*>("target");
    if(!target || target->ObjectDestroyed)
    {
        state = BTState::Failure;
        return state;
    }

    if(m_enemy.IsAttacking)
    {
        state = BTState::Running;
        return state;
    }

    glm::vec3 currentPos = m_enemy.transform.GetLocalPosition();
    glm::vec3 targetPos = target->transform.GetLocalPosition();
    if(glm::distance(targetPos, currentPos) <= m_attackRange)
    {
        m_enemy.Animator3D.PlayAnimation("Attack", true);
        m_enemy.IsAttacking = true;
        state = BTState::Success;
        return state;
    }

    state = BTState::Failure;
    return state;
}