#pragma once

#include <vector>
#include <memory>

#include "object/transform.h"

class Entity
{
public:
    virtual ~Entity() = default;
    
    //Scene graph
	std::vector<std::unique_ptr<Entity>> children;
	Entity* parent = nullptr;

	//Space information
	Transform transform;

	template<typename T, typename... Args>
	T& addChild(Args&&... args)
	{
		static_assert(std::is_base_of_v<Entity, T>, "T must derive from Entity");

		auto child = std::make_unique<T>(std::forward<Args>(args)...);
        child->parent = this;

		T& ref = *child;
		children.emplace_back(std::move(child));
		return ref;
	}

	//Update transform if it was changed
	void updateSelfAndChild()
	{
		if (transform.IsDirty()) {
			forceUpdateSelfAndChild();
			return;
		}
			
		for (auto&& child : children)
		{
			child->updateSelfAndChild();
		}
	}

	//Force update of transform even if local space don't change
	void forceUpdateSelfAndChild()
	{
		if (parent)
			transform.ComputeModelMatrix(parent->transform.GetModelMatrix());
		else
			transform.ComputeModelMatrix();

		for (auto&& child : children)
		{
			child->forceUpdateSelfAndChild();
		}
	}
};