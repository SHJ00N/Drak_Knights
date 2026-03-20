#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

struct SpawnData
{
    glm::vec3 position;
    std::vector<glm::vec3> wayPoints;

    float respawnTime;
};

struct SpawnRunTime
{
    SpawnData data;

    class Enemy* currentEnemy = nullptr;

    float spawnTimer = 0.0f;
    bool isSpawn = false;
};

class EnemySpawnManager
{
public:
    void LoadSpawnDataFromFile(const std::string &path);
    void Init(class Scene *scene, class GameObject &target);
    void Update(class Scene *scene, class GameObject &target, float dt);

private:
    std::vector<SpawnRunTime> m_spawns;

    void spawnEnemy(class Scene *scene, class GameObject &target, SpawnRunTime &spawn);
};