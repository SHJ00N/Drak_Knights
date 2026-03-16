#include "object/behavior_tree/behavior/Bfind_target.h"
#include "object/game_object.h"
#include "object/interface/animatable.h"

FindTarget::FindTarget(GameObject &gameObject, GameObject &target, float radius) : m_object(gameObject), m_target(target), m_radius(radius)
{
    m_animatable = dynamic_cast<Animatable*>(&m_object);
}

BTState FindTarget::Evaluate(float dt)
{
    if(m_target.ObjectDestroyed)
    {
        state = BTState::Failure;
        return state;
    }
    
    // calculate distance
    glm::vec3 currentPos = m_object.transform.GetLocalPosition();
    glm::vec3 targetPos = m_target.transform.GetLocalPosition();
    float distance = glm::distance(targetPos, currentPos);

    auto target = GetNodeData<GameObject*>("target");
    if(target)
    {
        if(distance <= m_radius)
        {
            if(m_animatable) m_animatable->Animator3D.PlayAnimation("Run");
            state = BTState::Success;
            return state;
        }
        else
        {
            RemoveNodeData("target");
            state = BTState::Failure;
            return state;
        }
    }

    // check distance
    if(distance <= m_radius)
    {
        SetNodeData("target", &m_target);
        if(m_animatable) m_animatable->Animator3D.PlayAnimation("Run");
        state = BTState::Success;
        return state;
    }

    state = BTState::Failure;
    return state;
}