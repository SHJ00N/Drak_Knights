#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "object/behavior_tree/BTree.h"

class Enemy;

class EnemyBT : public BTree
{
public:
    EnemyBT(Enemy &enemy, std::vector<glm::vec3> &wayPoints, GameObject &target);
    std::unique_ptr<BTNode> SetupTree() override;
private:
    Enemy &m_enemy;
    GameObject &m_target;
    std::vector<glm::vec3> &m_wayPoints;
    float m_speed;
    float m_radius;
    float m_attackRange;
};