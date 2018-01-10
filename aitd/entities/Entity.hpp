#pragma once

#include "ComponentPool.hpp"
#include <cstdint>
#include <tuple>
#include <bitset>
#include <iostream>
#include <functional>
#include <memory>

#define MAX_COMPONENTS 100

typedef uint32_t PoolIndex;

template <typename T>
class Component;
template <typename T>
class ComponentHandle;

class EntityManager;

////////////////////////////////////////////////////////////////
//
// Entity
//

class Entity {

public:

	struct Id {

		Id() : valid(false) {}
		Id(uint32_t idx) : id(idx), valid(true) {}

		uint32_t id;
		bool valid;

		uint32_t index() { return id; }

		inline bool operator== (const Id& _id_) {
			id == _id_.id;
		}
	};

	/**
	 * Id of an invalid Entity.
	 */
	static const Id INVALID;

	Entity(Id id) : _id(id) {}
	Entity(EntityManager *em, Id id) : _em(em), _id(id) {}

	//std::bitset<MAX_COMPONENTS> component_mask() const;

	Id id() { return _id; }

	bool valid() {
		return _id.valid;
	}

	template <typename C>
	void has_component();

	template <typename C, typename ... Args>
	ComponentHandle<C> assign(Args && ... args);

	template <typename C>
	void remove();

	template <typename C>
	ComponentHandle<C> component();

	template <typename ... Components>
	std::tuple<ComponentHandle<Components>...> components();

	std::bitset<MAX_COMPONENTS> component_mask() const;

protected:

	Id _id = INVALID;
	EntityManager *_em = nullptr;

};

////////////////////////////////////////////////////////////////
//
// Entity Manager
//


// So we have a bunch of "networked" ids that are pre-allocated and waiting to be
// used (1000). This can only be used-freed by networked entities. The rest are
// local


class EntityManager {

public:

	typedef std::shared_ptr<EntityManager> Ptr;
	typedef std::bitset<MAX_COMPONENTS> ComponentMask;

	static uint32_t max_networked_ids;
	static uint32_t entity_count;
	static uint32_t entity_networked_count;
	static uint32_t entity_local_count;

	uint32_t generateNetworkedId() {
		uint32_t id = ++EntityManager::entity_networked_count;
		assert(id < EntityManager::max_networked_ids);
		return id;
	}

	uint32_t generateLocalId() {
		uint32_t id = ++EntityManager::entity_local_count;
		assert(id > EntityManager::max_networked_ids);
		return id;
	}

	// TODO put this in namespace/class
	Entity::Id generateNewId(bool networked) {

		Entity::Id id;
		if (networked) {
			id.id = generateNetworkedId();
		}
		else {
			id.id = generateLocalId();
		}

		id.valid = true;
		return id.index();
	}

	Entity::Id getFreeId(bool networked) {

		uint32_t index;
		if (networked) {
			assert(not _freeNetworkedIdList.empty());
			index = _freeNetworkedIdList.back();
			_freeNetworkedIdList.pop_back();
		}
		else {
			assert(not _freeLocalIdList.empty());
			index = _freeLocalIdList.back();
			_freeLocalIdList.pop_back();
		}
		return Entity::Id(index);
	}

	void releaseId(uint32_t id) {
		if (id < EntityManager::max_networked_ids) {
			_freeLocalIdList.push_back(id);
		}
		else {
			_freeNetworkedIdList.push_back(id);
		}
	}

public:

	EntityManager()  {


		_valid_entities.resize(EntityManager::max_networked_ids);
		_entity_component_mask.resize(EntityManager::max_networked_ids);
		for (BasePool *pool : _component_pools)
			if (pool) pool->expand(EntityManager::max_networked_ids);
	};


	//TODO: generate networked entities
	// or local entities separately
	// same but ids in different ranges


	// Server ONLY generates global ids

	// Client ONLY generates local ids

	// So all non networked entities will have the upper bits to zero
	// Networked entities will have to upper

	// When client requests for an id, it can either get
	// the upper bits or the lower bits depending on which type

	Entity createFromId(int index) {

		assert(index < EntityManager::max_networked_ids);
		Entity::Id id(index);
		Entity entity(this, id);

		_valid_entities[id.index()] = true;
		EntityManager::entity_count++;
		return entity;
	}

	Entity createNetworked() {
		return create(true);
	}

	Entity createLocal() {
		return create(false);
	}

	bool isIdListEmpty(bool networked) {
		if (networked)
			return _freeNetworkedIdList.size() == 0;
		else
			return _freeLocalIdList.size() == 0;
	}

	bool valid(Entity::Id id) {
		return _valid_entities[id.id];
	}

	void destroy(Entity::Id id) {
		assert(valid(id));
		releaseId(id.id);
		_entity_component_mask[id.index()].reset();
		_valid_entities[id.id] = false;
	}

	Entity get(Entity::Id id) {
		assert(valid(id));
		return Entity(this, id);
	}

	template <typename CType>
	ComponentHandle<CType> component(Entity::Id id) {
		assert(valid(id));
		PoolIndex index = Component<CType>::poolIndex();
		BasePool *pool = _component_pools[index];
		//TODO: assert stuff with pool
		return ComponentHandle<CType>(id, this);
	}

	template <typename CType>
	CType *getComponentPtr(Entity::Id id) {
		assert(valid(id));
		BasePool *pool = getComponentPool<CType>();
		return static_cast<CType*>(pool->get(id.index()));
	}

	template <typename CType>
	Pool<CType> *getComponentPool() {
		PoolIndex index = Component<CType>::poolIndex();
		assert(index < _component_pools.size());
		return static_cast<Pool<CType>*>(_component_pools[index]);
	}

	template <typename C, typename ... Args>
	ComponentHandle<C> assign(Entity::Id id, Args && ... args);

	template <typename C>
	void remove(Entity::Id id) {
		valid(id);
		PoolIndex poolIndex = Component<C>::poolIndex();
		Pool<C> *pool = static_cast<Pool<C>* >(_component_pools[poolIndex]);
	        pool->destroy(id.index());
	}

	size_t capacity() const { return _entity_component_mask.size(); }

	ComponentMask component_mask(Entity::Id id) {
		valid(id);
		return _entity_component_mask.at(id.index());
	}


	template <typename CType>
	ComponentMask component_mask() {
		ComponentMask mask;
		mask.set(Component<CType>::poolIndex());
		return mask;
	}
	template <typename C1, typename C2, typename ... Components>
	ComponentMask component_mask() {
		return component_mask<C1>() | component_mask<C2, Components ...>();
	}


	template <typename T> struct identity { typedef T type; };

	template <typename ... Components>
	void each(typename identity<std::function<void(Entity entity, Components&...)> >::type f) {
		auto mask = component_mask<Components...>();
		auto view = ComponentsView<Components...>(this, mask);
		return view.each(f);
	}



	// template <typename ... Components>b
	// void each(std::function<void(Entity entity, Components&...)> f) {
	// 	auto mask = component_mask<Components...>();
	// 	auto view = ComponentsView<Components...>(this, mask);
	// 	return view.each(f);
	// }

protected:


	Entity create(bool networked = false) {

		Entity::Id id;

		if (isIdListEmpty(networked)) {
			id = generateNewId(networked);
		}
		else {
			id = getFreeId(networked);
		}

		Entity entity(this, id);
		reserveEntityData(id.index());
		_valid_entities[id.index()] = true;

		//event_manager.broadcast<EntityCreatedEvent>(entity);
		return entity;
	}

	template <typename ... Components>
	class ComponentsView {

	public:

		//Iterator over entities
		class Iterator {
		public:

			Iterator(EntityManager *em,
				 const ComponentMask mask,
				 uint32_t index) : _em(em),
						   _mask(mask),
						   _capacity(em->capacity()),
						   _i(index) {

				find_next();
			}

			 Iterator &operator ++() {
				 ++_i;
				 find_next();
				 return *static_cast<Iterator*>(this);
			 }

			void find_next() {
				//jump elements until we find the next with matching mask
				while(_i < _capacity &&
				      (_em->_entity_component_mask[_i] & _mask) != _mask) {
					_i++;
				}
			}

			bool operator == (const Iterator& rhs) const { return _i == rhs._i; }
			bool operator != (const Iterator& rhs) const { return _i != rhs._i; }
			Entity operator * () { return Entity(_em, Entity::Id(_i)); }
			const Entity operator * () const { return Entity(_em, Entity::Id(_i)); }
		protected:

			EntityManager *_em;
			size_t _capacity;
			ComponentMask _mask;
			uint32_t _i;

		};

		ComponentsView(EntityManager *em, ComponentMask mask) : _em(em), _mask(mask) {}

		Iterator begin() { return Iterator(_em, _mask, 0); }
		Iterator end() { return Iterator(_em, _mask, uint32_t(_em->capacity())); }
		const Iterator begin() const { return Iterator(_em, _mask, 0); }
		const Iterator end() const { return Iterator(_em, _mask, uint32_t(_em->capacity())); }


		//shall we force this to be the same as Components?
		template <typename ... OtherComponents>
		void each(std::function<void(Entity entity, OtherComponents&...)> f) {
			for (auto it : *this) {
				f(it, *(it.template component<Components>().get())...);
			}
		}


	protected:

		EntityManager * _em;
		ComponentMask _mask;
	};

	void reserveEntityData(uint32_t id) {
		if (_valid_entities.size() <= id) {
			_valid_entities.resize(id+1);
			_entity_component_mask.resize(id+1);
			for (BasePool *pool : _component_pools)
				if (pool) pool->expand(id + 1);
		}
	}

	template <typename C>
	Pool<C>* reserveComponentData() {

		PoolIndex poolIndex = Component<C>::poolIndex();

		if (poolIndex >= _component_pools.size()) {
			_component_pools.resize(poolIndex + 1);
		}

		if (not _component_pools[poolIndex]) {
			_component_pools[poolIndex] = new Pool<C>();
			_component_pools[poolIndex]->expand(EntityManager::max_networked_ids +
												EntityManager::entity_local_count);
		}

		return static_cast<Pool<C>*>(_component_pools[poolIndex]);
	}

	std::vector<BasePool*> _component_pools;
	std::vector<ComponentMask> _entity_component_mask;
	std::vector<bool> _valid_entities;
        std::vector<uint32_t> _freeNetworkedIdList;
	std::vector<uint32_t> _freeLocalIdList;

};

////////////////////////////////////////////////////////////////
//
// Component Handle
//

template <typename CType>
class ComponentHandle {

public:
	ComponentHandle() : _em(nullptr) {}
	ComponentHandle(Entity::Id id, EntityManager *em) : _id(id), _em(em) {}

	bool valid() {
		return _em && _em->valid(_id);
	}

	CType *get() {
		assert(valid());
		return _em->getComponentPtr<CType>(_id);
	}

	/**
	 * Remove the component from its entity and destroy it.
	 */
	void remove() {
		assert(valid());
		_em->remove<CType>();
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
	static PoolIndex pool_index_counter;
};

template <typename CType>
class Component : public BaseComponent {
public:

	static PoolIndex poolIndex() {
		static PoolIndex index = pool_index_counter++;
		assert(index < MAX_COMPONENTS);
		return index;
	}
};


template <typename C>
void Entity::has_component() {
	assert(valid());
	return _em->has_component<C>();
}

template <typename C, typename ... Args>
ComponentHandle<C> Entity::assign(Args && ... args) {
	assert(valid());
	return _em->assign<C>(_id, std::forward<Args>(args) ...);
}

template <typename C>
void Entity::remove() {
	assert(valid() && has_component<C>());
	_em->remove<C>(_id);
}

template <typename C>
ComponentHandle<C> Entity::component() {
	assert(valid());
	return _em->component<C>(_id);
}

template <typename ... Components>
std::tuple<ComponentHandle<Components>...> Entity::components() {
	assert(valid());
	return _em->components<Components...>();
}


template <typename C, typename ... Args>
ComponentHandle<C> EntityManager::assign(Entity::Id id, Args && ... args) {
	valid(id);
	PoolIndex poolIndex = Component<C>::poolIndex();

	//check (wth mask) that the entity does not have this component already

	std::cout << "checking entity:"<< id.index() << "component:" << poolIndex << std::endl;

	assert(!_entity_component_mask[id.index()].test(poolIndex));

	BasePool *pool = reserveComponentData<C>();
	void *comp = pool->get(id.index());
	::new(comp) C(std::forward<Args>(args)...);
	_entity_component_mask[id.index()].set(poolIndex);

	ComponentHandle<C> component(id, this);

	// TODO send message of comp. creation

	return component;
}
