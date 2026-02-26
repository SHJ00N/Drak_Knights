#include <iostream>
#include <memory>

#include "scene/game_play_scene.h"
#include "resource_manager.h"
#include "render/renderer/terrain_renderer.h"
#include "light/direction_light.h"
#include "world/world.h"
#include "object/player.h"
#include "object/bone_demo_obj.h"
#include "object/weapon.h"
#include "object/game_object.h"
#include "particle/blood_particle.h"

GamePlayScene::GamePlayScene(unsigned int width, unsigned int height) : Scene(width, height)
{
    // set render type
    renderType = RenderType::Deferred;
}

GamePlayScene::~GamePlayScene()
{
    delete MainCamera; // parent camera pointer
    // parent lights pointer vector
    for(const auto& iter : Lights)
    {
        delete iter;
    }
    Lights.clear();
    // current scene class pointers
    delete player;
    delete boneDemoObj;
    delete world;
    delete terrainRenderer;
    delete sword;
    // delete IBL textures
    IBLtextures.Destroy();
}

void GamePlayScene::Init()
{
    // load shaders
    ResourceManager::LoadShader("shaders/model_shader/boneMesh.vert", "shaders/model_shader/mesh.frag", nullptr, nullptr, nullptr, "boneModel");
    ResourceManager::LoadShader("shaders/model_shader/staticMesh.vert", "shaders/model_shader/mesh.frag", nullptr, nullptr, nullptr, "staticModel");
    ResourceManager::LoadShader("shaders/terrain_shader/terrain.vert", "shaders/terrain_shader/terrain.frag", nullptr, "shaders/terrain_shader/terrain.tcs", "shaders/terrain_shader/terrain.tes", "terrainShader");
    ResourceManager::LoadShader("shaders/csm_shader/terrainShadow.vert", "shaders/csm_shader/cascadedShadow.frag", "shaders/csm_shader/cascadedShadow.geom", "shaders/csm_shader/terrainShadow.tesc", "shaders/csm_shader/terrainShadow.tese", "terrainShadow");
    ResourceManager::LoadShader("shaders/sample_shader/blood.vert", "shaders/sample_shader/blood.frag", nullptr, nullptr, nullptr, "blood");
    // load compute shaders
    ResourceManager::LoadComputeShader("shaders/sample_shader/compute.comp", "bloodCompute");

    // load texture
    ResourceManager::LoadTerrainTexture("resources/texture/Diffuse_16BIT_PNG.png", "resources/texture/CombinedNormal_8BIT_PNG.png", "resources/texture/Roughness_16BIT_PNG.png", "snowField", false);
    ResourceManager::LoadTexture("resources/texture/blood1.png", true, "blood_splatter", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    // create world and renderer
    world = new World("resources/texture/Heightmap_16BIT_PNG.png", 1.0f, 640.0f, 4, 64.0f, 40);
    terrainRenderer = new TerrainRenderer(ResourceManager::GetShader("terrainShader"), ResourceManager::GetShader("terrainShadow"), *world, ResourceManager::GetTerrainTexture("snowField"));
    
    // load models
    ResourceManager::LoadModel("resources/object/knight2/SKM_DKM_Full.fbx", false, "knight");
    ResourceManager::LoadModel("resources/object/SM_DKM_Sword.fbx", false, "sword");
    // load animations
    ResourceManager::LoadAnimation("resources/animation/knight2/Anim_DKM_Idle_Alert.fbx", ResourceManager::GetModel("knight"), "knight_idle");
    ResourceManager::LoadAnimation("resources/animation/knight2/Anim_DKM_Walk_Alert_Fwd.fbx", ResourceManager::GetModel("knight"), "knight_walk");
    ResourceManager::LoadAnimation("resources/animation/knight2/Anim_DKM_Run_Alert_Fwd.fbx", ResourceManager::GetModel("knight"), "knight_run");
    ResourceManager::LoadAnimation("resources/animation/knight2/Anim_DKM_Attack_01.fbx", ResourceManager::GetModel("knight"), "knight_attack1", false);
    ResourceManager::LoadAnimation("resources/animation/knight2/Anim_DKM_Attack_02.fbx", ResourceManager::GetModel("knight"), "knight_attack2", false);
    ResourceManager::LoadAnimation("resources/animation/knight2/Anim_DKM_Attack_03.fbx", ResourceManager::GetModel("knight"), "knight_attack3", false);
    ResourceManager::LoadAnimation("resources/animation/knight2/Stand To Roll.fbx", ResourceManager::GetModel("knight"), "knight_roll", false);
    // create IBL textures
    IBLtextures = IBLGenerator::GenerateIBLFromHDR("resources/texture/galaxy_hdr.png");
    // create main camera
    MainCamera = new Camera();
    // create lights
    Lights.push_back(new DirLight(LightType::Direction, glm::vec3(-0.6f, -1.0f, 0.7f), glm::vec3(0.4f, 0.6f, 1.0f), 1.0f));

    // create game objects and set animator
    player = new Player(ResourceManager::GetModel("knight"), ResourceManager::GetShader("boneModel"), glm::vec3(2046.0f, 0.0f, 2046.0f), glm::vec3(0.01f), glm::vec3(0.0f, 0.0f, 0.0f));
    player->AddAnimation("Idle", &ResourceManager::GetAnimation("knight_idle"));
    player->AddAnimation("Walk", &ResourceManager::GetAnimation("knight_walk"));
    player->AddAnimation("Run", &ResourceManager::GetAnimation("knight_run"));
    player->AddAnimation("Attack1", &ResourceManager::GetAnimation("knight_attack1"));
    player->AddAnimation("Attack2", &ResourceManager::GetAnimation("knight_attack2"));
    player->AddAnimation("Attack3", &ResourceManager::GetAnimation("knight_attack3"));
    player->AddAnimation("Roll", &ResourceManager::GetAnimation("knight_roll"));
    renderables.push_back(player); // add to renderables list
    gameObjects.push_back(player); // add to game objects list

    sword = new Weapon(ResourceManager::GetModel("sword"), ResourceManager::GetShader("staticModel"), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    player->AttachWeapon(sword);
    renderables.push_back(sword);

    boneDemoObj = new BoneDemoObj(ResourceManager::GetModel("knight"), ResourceManager::GetShader("staticModel"), glm::vec3(2146.0f, 0.0f, 2046.0f), glm::vec3(0.01f), glm::vec3(270.0f, 90.0f, 0.0f));
    renderables.push_back(boneDemoObj); // add to renderables list
    gameObjects.push_back(boneDemoObj); // add to game objects list

    Start();
}

void GamePlayScene::Start()
{
    glm::vec3 demoWorldPos = boneDemoObj->transform.GetGlobalPosition();
    boneDemoObj->SetWorldHeight(world->GetWorldHeight(demoWorldPos.x, demoWorldPos.z));
}

void GamePlayScene::Update(float dt)
{
    // update world
    world->Update(MainCamera->cameraPos);
    // update all game objects
    ObjectUpdateContext context{dt, world, MainCamera};
    for(auto& object : gameObjects)
    {
        object->Update(context);
    }

    for(auto& particle : particles)
    {
        particle->Update(dt);
    }
    // update camera
    MainCamera->Update(player->GetSoketGlobalPosition("center"), dt);
}

void GamePlayScene::ProcessInput(float dt)
{
    // attack input
    if(MouseButtonLeft)
    {
        player->RequestAttack();
    }
    // roll input
    if(Keys[GLFW_KEY_SPACE])
    {
        player->RequestRoll();
    }
    // movement input
    player->RequestMove(glm::vec3(Keys[GLFW_KEY_D] - Keys[GLFW_KEY_A], 0.0f, Keys[GLFW_KEY_W] - Keys[GLFW_KEY_S]), Keys[GLFW_KEY_LEFT_SHIFT]);

    if(MouseButtonRight)
    {
        auto& bloodParticle = player->addChild<BloodParticle>(ResourceManager::GetShader("blood"), ResourceManager::GetComputeShader("bloodCompute"), ResourceManager::GetTexture("blood_splatter"), 
            player->GetSoketLocalPosition("Center"), glm::vec3(1.0f), 1, 1, 1);
        bloodParticle.Init();
        particles.push_back(&bloodParticle);
    }
}

void GamePlayScene::End()
{

}