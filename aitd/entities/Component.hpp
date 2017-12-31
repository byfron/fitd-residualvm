#pragma once

#include "Entity.hpp"

template <typename CType>
class ComponentHandle {
       
public:
	ComponentHandle() : _em(nullptr) {}


	bool valid() {
		return _em && _em->valid(_id);
	}

	CType *get() {
		assert(valid());
		return _em->getComponent<CType>(_id);
	}
	
	/**
	 * Remove the component from its entity and destroy it.
	 */
	void remove() {

	}

	/**
	 * Returns the Entity associated with the component
	 */
	Entity entity() {
		assert(valid());
		return _em->get(_id);
	}
	
	
protected:
	Entity::Id _id;
	EntityManager *_em;
};


class BaseComponent {

public:
	typedef size_t PoolIndex;
	static PoolIndex pool_index_counter;
}

template <typename CType>
class Component : public BaseComponent {
public:

	static PoolIndex poolIndex() {
		static PoolIndex index = pool_index_counter_++;
		assert(index < MAX_COMPONENTS);
		return index;
	}
		
};
