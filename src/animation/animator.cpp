#include "animation/animator.h"
#include "animation/animation.h"

#include <iostream>

Animator::Animator(){
    m_CurrentTime = 0.0;
    m_CurrentAnimation = nullptr;

    m_FinalBoneMatrices.reserve(100);
    m_GlobalBoneMatrices.reserve(100);

    for(int i = 0; i < 100; i++)
    {
        m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
        m_GlobalBoneMatrices.push_back(glm::mat4(1.0f));
    }
}

void Animator::registAnimation(const std::string &name, Animation *animation)
{
    m_Animations[name] = animation;
}

Animation* Animator::GetAnimation(const std::string &name)
{
    if(m_Animations.find(name) != m_Animations.end())
    {
        return m_Animations[name];
    }
    return nullptr;
}

bool Animator::IsAnimationFinished()
{
    bool isFinished = m_animationIsFinished;
    m_animationIsFinished = false; // reset the flag after checking
    return isFinished;
}

void Animator::UpdateAnimation(float dt){
    if(!m_CurrentAnimation) return;

    float prevTime = m_CurrentTime;
    // update current animation time
    m_DeltaTime = dt;
    m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;

    if(!m_CurrentAnimation->IsLooping())
    {
        if(m_CurrentTime >= m_CurrentAnimation->GetDuration())
        {
            m_CurrentTime = std::min(m_CurrentTime, m_CurrentAnimation->GetDuration() - 0.0001f);
            m_animationIsFinished = true;
        }
        else
        {
            m_animationIsFinished = false;
        }
    } 
    else 
    {
        m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
        m_animationIsFinished = (m_CurrentTime < prevTime); // animation looped
    }
    
    // reset root motion delta on animation loop
    if(m_CurrentAnimation->IsLooping() && m_CurrentTime < prevTime)
    {
        m_currentRootInitialized = false; 
    }

    // if request animation change, animation blending perform
    if(m_isBlending)
    {
        // update next animation time
        m_nextTime += m_nextAnimation->GetTicksPerSecond() * dt;
        m_nextTime = fmod(m_nextTime, m_nextAnimation->GetDuration());
        // calculate blend factor
        m_blendTime += dt;
        float weight = glm::clamp(m_blendTime / m_blendDuration, 0.0f, 1.0f);

        // end blending
        if(weight >= 1.0f)
        {
            m_CurrentAnimation = m_nextAnimation;
            m_CurrentTime = m_nextTime;
            m_nextAnimation = nullptr;
            m_isBlending = false;

            CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
            return;
        }

        // blending
        CalculateBoneTransformBlended(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f), weight);

        // calculate delta
        m_rootMotionDelta = ExtractRootMotionDelta(m_nextAnimation, m_prevNextRootPosition, m_nextRootInitialized);
    }
    else
    {
        CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
        m_rootMotionDelta = ExtractRootMotionDelta(m_CurrentAnimation, m_prevCurrentRootPosition, m_currentRootInitialized);
    }
}

void Animator::PlayAnimation(const std::string &name, bool force)
{
    PlayAnimation(m_Animations[name], force);
}

void Animator::PlayAnimation(Animation *pAnimation, bool force){
    if(!pAnimation) return;

    // first call
    if(!m_CurrentAnimation)
    {
        m_CurrentAnimation = pAnimation;
        m_CurrentTime = 0.0f;
        m_currentRootInitialized = false;
        return;
    }

    // already playing the same animation
    if(m_CurrentAnimation == pAnimation && !force && !m_animationIsFinished) return;

    // if force play, immediately switch to new animation without blending
    if(force)
    {
        m_isBlending = false;
        m_nextAnimation = nullptr;

        m_CurrentAnimation = pAnimation;
        m_CurrentTime = 0.0f;
        m_currentRootInitialized = false;
        return;
    }

    // animation blending setup
    if(m_CurrentAnimation != pAnimation && m_nextAnimation != pAnimation)
    {
        m_nextAnimation = pAnimation;
        m_nextTime = 0.0f;
        m_blendTime = 0.0f;
        m_nextRootInitialized = false;
        m_isBlending = true;
    }
}

glm::vec3 Animator::ExtractRootMotionDelta(Animation *animation, glm::vec3 &prevRootPos, bool &initalized)
{
    Bone *bone = animation->FindBone("root_$AssimpFbx$_Translation");
    if(!bone) return glm::vec3(0.0f);

    glm::vec3 position = bone->GetLocalPosition();
    // first frame, initialize previous root position
    if(!initalized)
    {
        prevRootPos = position;
        initalized = true;
        return glm::vec3(0.0f);
    }

    // calculate delta
    glm::vec3 delta = position - prevRootPos;
    prevRootPos = position;

    return glm::vec3(delta.x, 0.0f, delta.z);
}

glm::vec3 Animator::GetRootMotionDelta()
{
    glm::vec3 delta = m_rootMotionDelta;
    m_rootMotionDelta = glm::vec3(0.0f);
    return delta;
}

void Animator::CalculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform){
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;

    Bone *Bone = m_CurrentAnimation->FindBone(nodeName);

    if(Bone){
        Bone->Update(m_CurrentTime);
        nodeTransform = Bone->GetLocalTransform();
    }

    // remove translation of root node
    if(nodeName == "root_$AssimpFbx$_Translation")
    {
        nodeTransform[3][0] = 0.0f;
        nodeTransform[3][2] = 0.0f;
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
    if(boneInfoMap.find(nodeName) != boneInfoMap.end()){
        int index = boneInfoMap[nodeName].id;
        glm::mat4 offset = boneInfoMap[nodeName].offset;
        m_FinalBoneMatrices[index] = globalTransformation * offset;

        // for soket
        m_GlobalBoneMatrices[index] = globalTransformation;
    }

    for(int i = 0; i < node->childrenCount; i++)
        CalculateBoneTransform(&node->children[i], globalTransformation);
}

void Animator::CalculateBoneTransformBlended(const AssimpNodeData *node, glm::mat4 parentTransform, float weight)
{
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;
    // bone data
    Bone *curBone = m_CurrentAnimation->FindBone(nodeName);
    Bone *nextBone = m_nextAnimation->FindBone(nodeName);
    if(curBone) curBone->Update(m_CurrentTime);
    if(nextBone) nextBone->Update(m_nextTime);

    if(curBone && nextBone)
    {
        // blended local transform
        glm::vec3 position = glm::mix(curBone->GetLocalPosition(), nextBone->GetLocalPosition(), weight);
        glm::quat rotation = glm::slerp(curBone->GetLocalRotation(), nextBone->GetLocalRotation(), weight);
        glm::vec3 scale = glm::mix(curBone->GetLocalScale(), nextBone->GetLocalScale(), weight);

        glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 R = glm::toMat4(rotation);
        glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
        nodeTransform = T * R * S;
    } 
    else if(curBone)
    {
        nodeTransform = curBone->GetLocalTransform();
    }
    else if(nextBone)
    {
        nodeTransform = nextBone->GetLocalTransform();
    }

    // remove translation of root node
    if(nodeName == "root_$AssimpFbx$_Translation")
    {
        nodeTransform[3][0] = 0.0f;
        nodeTransform[3][2] = 0.0f;
    }

    glm::mat4 globalBlendedTransformation = parentTransform * nodeTransform;

    auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
    if(boneInfoMap.find(nodeName) != boneInfoMap.end()){
        int index = boneInfoMap[nodeName].id;
        glm::mat4 offset = boneInfoMap[nodeName].offset;
        m_FinalBoneMatrices[index] = globalBlendedTransformation * offset;

        // for soket
        m_GlobalBoneMatrices[index] = globalBlendedTransformation;
    }

    for(int i = 0; i < node->childrenCount; i++)
        CalculateBoneTransformBlended(&node->children[i], globalBlendedTransformation, weight);
}

std::vector<glm::mat4> Animator::GetFinalBoneMatrices(){
    return m_FinalBoneMatrices;
}

std::vector<glm::mat4> Animator::GetGlobalBoneMatrices(){
    return m_GlobalBoneMatrices;
}