#pragma once

#include <glm/glm.hpp>

#include "object/behavior_tree/BTNode.h"

class GameObject;
class Animatable;

class FindTarget : public BTNode
{
public:
    // constructor
    FindTarget(GameObject &gameObject, GameObject &target, float radius);
    // override function
    BTState Evaluate(float dt) override;

private:
    // object and target points
    GameObject &m_object;
    GameObject &m_target;
    // animator
    Animatable *m_animatable = nullptr;

    float m_radius;
};