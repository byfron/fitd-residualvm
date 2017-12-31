#include "Entity.hpp"

std::bitset<MAX_COMPONENTS> Entity::component_mask() const {
	_em->component_mask(_id);
}
