#include "ResourceManager.hpp"

namespace pumpkin {
	std::unordered_map<uint32_t, std::unordered_map<uint32_t, Resource::Ptr>> ResourceManager::_resource_map;
}
