#pragma once

#include "object/behavior_tree/BTNode.h"

class Selector : public BTNode
{
public:
    // constructors
    Selector() : BTNode() { }
    Selector(std::vector<std::unique_ptr<BTNode>>&& children) : BTNode(std::move(children)) { }

    // override function
    BTState Evaluate(float dt) override
    {
        for(auto &node : children)
        {
            switch(node->Evaluate(dt))
            {
                case BTState::Failure:
                    break;
                case BTState::Success:
                    state = BTState::Success;
                    return state;
                case BTState::Running:
                    state = BTState::Running;
                    return state;
            }
        }

        state = BTState::Failure;
        return state;
    }
};