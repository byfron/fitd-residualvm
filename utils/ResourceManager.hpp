#pragma once

#include <memory>
#include <unordered_map>
#include <assert.h>
#include <Eigen/Dense>

class Resource {
public:
	virtual void init() = 0;
	typedef std::shared_ptr<Resource> Ptr;

	uint32_t getId() { return m_id; }

protected:
	uint32_t m_id;
};

class ResourceManager {

public:

	template <typename T>
	static typename T::Ptr getResource(uint32_t id) {
		if (resourceExists(T::type(), id)) {
			return std::static_pointer_cast<T>(getResourcePtr(T::type(), id));
		}
		return nullptr;
	}

	static bool resourceExists(uint32_t type, uint32_t id) {
		return _resource_map[type].count(id) > 0;
	}

	template <typename T, typename TConf>
	static void pushResource(const TConf & cfg) {
		Resource::Ptr r = std::make_shared<T>(cfg);
		uint32_t res_id = r->getId();

		if (_resource_map[T::type()].count(res_id) == 0) {
			_resource_map[T::type()][res_id] = r;
		}
		else {
			//log std::cout << "error id existing" <<std::endl;
			assert(false);
		}
	}

	static Resource::Ptr getResourcePtr(uint32_t type, uint32_t index) {
		assert(_resource_map[type].count(index));
		return _resource_map[type][index];
	}

private:

	// This is simple but shitty. If we have types we can store them in separate pools
	// and avoid dynamically creating resources
	static std::unordered_map<uint32_t,
							  std::unordered_map<uint32_t, Resource::Ptr>> _resource_map;
};

