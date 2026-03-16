#pragma once

#include <memory>

#include "object/game_object.h"
#include "object/behavior_tree/BTNode.h"

class BTree : public GameObject
{   
public:
    // override functions
    void Init() override
    {
        m_root = SetupTree();
    }
    void Update(const ObjectUpdateContext &context) override
    {
        if(m_root)
        {
            m_root->Evaluate(context.deltaTime);
        }
    }

protected:
    virtual std::unique_ptr<BTNode> SetupTree() = 0;

private:
    std::unique_ptr<BTNode> m_root;
};