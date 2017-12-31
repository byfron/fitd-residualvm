#include "System.hpp"

BaseSystem::Family BaseSystem::_family_counter;
uint32_t EntityManager::entity_count = 0;
uint32_t EntityManager::max_networked_ids = 1000;
uint32_t EntityManager::entity_local_count = EntityManager::max_networked_ids;
uint32_t EntityManager::entity_networked_count = 0;
PoolIndex BaseComponent::pool_index_counter = 0;

BaseSystem::~BaseSystem() {
}

void SystemManager::update_all(float dt) {
	assert(_initialized && "SystemManager::configure() not called");
	for (auto &pair : _systems) {
		pair.second->update(_em, _evm, dt);
	}
}
