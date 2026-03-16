#pragma once

#include "object/behavior_tree/BTNode.h"

class Sequence : public BTNode
{
public:
    // constructors
    Sequence() : BTNode() { }
    Sequence(std::vector<std::unique_ptr<BTNode>>&& children) : BTNode(std::move(children)) { }

    // override function
    BTState Evaluate(float dt) override
    {
        bool anyChildIsRunning = false;

        for(auto &node : children)
        {
            switch(node->Evaluate(dt))
            {
                case BTState::Failure:
                    state = BTState::Failure;
                    return state;
                case BTState::Success:
                    break;
                case BTState::Running:
                    anyChildIsRunning = true;
                    break;
            }
        }

        state = anyChildIsRunning ? BTState::Running : BTState::Success;
        return state;
    }
};