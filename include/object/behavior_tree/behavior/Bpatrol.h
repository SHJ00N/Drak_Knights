#pragma once

#include <glm/glm.hpp>

#include "object/behavior_tree/BTNode.h"

class GameObject;
class Animatable;

class Patrol : public BTNode
{
public:
    // constructor
    Patrol(GameObject &gameObject, std::vector<glm::vec3> &wayPoints, float speed);
    // override function
    BTState Evaluate(float dt) override;

private:
    // object and target points
    GameObject &m_object;
    std::vector<glm::vec3> &m_wayPoints;
    // animator
    Animatable *m_animatable = nullptr;

    int m_currentWayPointIndex = 0;

    float m_speed;

    // waiting member
    float m_waitTime = 1.0f;
    float m_waitTimer = 0.0f;
    bool m_waiting = false;

    // object move to waypoint
    void Move(const glm::vec3 &wayPoint, float dt);
    float NormalizeAngle(float angle);
};