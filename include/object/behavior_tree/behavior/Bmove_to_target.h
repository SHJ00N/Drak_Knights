#pragma once

#include "object/behavior_tree/BTNode.h"

class GameObject;

class MoveToTarget : public BTNode
{
public:
    // constructor
    MoveToTarget(GameObject &gameObject, GameObject &target, float speed);
    // override function
    BTState Evaluate(float dt) override;

private:
    // object and target
    GameObject &m_object;
    GameObject &m_target;

    float m_speed;

    void Move(float dt);
    float NormalizeAngle(float angle);
};