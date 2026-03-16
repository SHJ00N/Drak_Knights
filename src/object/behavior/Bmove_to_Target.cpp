#include "object/behavior_tree/behavior/Bmove_to_target.h"
#include "object/game_object.h"

#include <glm/glm.hpp>

MoveToTarget::MoveToTarget(GameObject &gameObject, GameObject &target, float speed) : m_object(gameObject), m_target(target), m_speed(speed)
{
}

BTState MoveToTarget::Evaluate(float dt)
{
    glm::vec3 currentPos = m_object.transform.GetLocalPosition();
    glm::vec3 targetPos = m_target.transform.GetLocalPosition();

    // check distance to target
    if(glm::distance(currentPos, targetPos) > 0.01f)
    {
        Move(dt);
    }

    state = BTState::Running;
    return state;
}

void MoveToTarget::Move(float dt)
{
    glm::vec3 currentPos = m_object.transform.GetLocalPosition();
    glm::vec3 targetPos = m_target.transform.GetLocalPosition();

    glm::vec3 dir = targetPos - currentPos;
    dir.y = 0.0f;

    float dist = glm::length(dir);

    // prevent overshoot
    if(dist < m_speed * dt)
    {
        m_object.transform.SetLocalPosition(targetPos);
        return;
    }

    dir = glm::normalize(dir);

    // rotation
    glm::vec3 localRot = m_object.transform.GetLocalRotation();

    float targetYaw = glm::degrees(atan2(dir.x, dir.z));
    float delta = NormalizeAngle(targetYaw - localRot.y);

    if(fabs(delta) > 140.0f)
        localRot.y = targetYaw;
    else
        localRot.y += delta * dt * 6.0f;

    localRot.y = NormalizeAngle(localRot.y);
    m_object.transform.SetLocalRotation(localRot);

    // position
    glm::vec3 newPos = currentPos + dir * m_speed * dt;
    m_object.transform.SetLocalPosition(newPos);
}

float MoveToTarget::NormalizeAngle(float angle)
{
    while(angle > 180.0f) angle -= 360.0f;
    while(angle < -180.0f) angle += 360.0f;
    return angle;
}