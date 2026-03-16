#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <any>
#include <string>

enum class BTState
{
    Success,
    Failure,
    Running
};

class BTNode
{
public:
    BTState state = BTState::Failure;
    BTNode *parent = nullptr;

    // constructors and destructor
    BTNode() { }
    BTNode(std::vector<std::unique_ptr<BTNode>>&& children)
    {
        for(auto &child : children)
        {
            Attach(std::move(child));
        }
    }
    virtual ~BTNode() = default;

    // evaulate node state
    virtual BTState Evaluate(float dt)
    {
        return BTState::Failure;
    }

    // getter and setter
    template<typename T>
    void SetNodeData(const std::string &key, T value)
    {
        BTNode* node = this;

        // move to root
        while(node->parent) node = node->parent;

        node->m_nodeData[key] = value;
    }

    template<typename T>
    T GetNodeData(const std::string &key)
    {
        auto it = m_nodeData.find(key);

        if(it != m_nodeData.end())
            return std::any_cast<T>(it->second);
        
        if(parent)
            return parent->GetNodeData<T>(key);
        
        return nullptr;
    }

    bool RemoveNodeData(const std::string &key)
    {
        auto it = m_nodeData.find(key);

        if(it != m_nodeData.end())
        {
            m_nodeData.erase(it);
            return true;
        }

        if(parent)
            return parent->RemoveNodeData(key);

        return false;
    }

protected:
    std::vector<std::unique_ptr<BTNode>> children;

private:
    std::unordered_map<std::string, std::any> m_nodeData;

    // attach node to children
    void Attach(std::unique_ptr<BTNode> node)
    {
        node->parent = this;
        children.push_back(std::move(node));
    }
};