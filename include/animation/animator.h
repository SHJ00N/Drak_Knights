#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

class Animation;
class AssimpNodeData;

const float BLEND_DURATION = 0.15f;

class Animator{
public:
    Animator();
    void UpdateAnimation(float dt);
    void PlayAnimation(Animation *pAnimation, bool force = false);
    void CalculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform);
    void CalculateBoneTransformBlended(const AssimpNodeData *node, glm::mat4 parentTransform, float weight);
    std::vector<glm::mat4> GetFinalBoneMatrices();
    std::vector<glm::mat4> GetGlobalBoneMatrices();
    glm::vec3 GetRootMotionDelta();
    glm::vec3 ExtractRootMotionDelta(Animation *animation, glm::vec3 &prevRootPos, bool &initalized);
    void registAnimation(const std::string &name, Animation *animation);
    Animation* GetAnimation(const std::string &name);
    bool IsAnimationFinished();
private:
    // animation list
    std::unordered_map<std::string, Animation*> m_Animations;
    std::vector<glm::mat4> m_FinalBoneMatrices;
    // for soket
    std::vector<glm::mat4> m_GlobalBoneMatrices;
    Animation *m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;

    // blend member
    Animation *m_nextAnimation;
    float m_nextTime;
    float m_blendTime;
    float m_blendDuration = BLEND_DURATION;
    bool m_isBlending = false;

    // animtaion position delta
    glm::vec3 m_prevCurrentRootPosition = glm::vec3(0.0f);
    glm::vec3 m_prevNextRootPosition = glm::vec3(0.0f);
    glm::vec3 m_rootMotionDelta = glm::vec3(0.0f);
    bool m_currentRootInitialized = false;
    bool m_nextRootInitialized = false;

    bool m_animationIsFinished = false;
};