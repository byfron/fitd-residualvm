#pragma once

#include <memory>
#include <unordered_map>
#include <assert.h>
#include <Eigen/Dense>

#define TEXTURE_ATLAS_RESOURCE_TYPE 0
#define SHADER_RESOURCE_TYPE 1
#define COLOR_PALETTE_RESOURCE_TYPE 2

#define OBJECT_PALETTE_ID 0
#define BACKGROUND_PALETTE_ID 1

class Resource {
public:
	Resource(uint32_t id) : m_id(id) {}
	virtual void init() = 0;
	typedef std::shared_ptr<Resource> Ptr;

	uint32_t getId() { return m_id; }

protected:
	uint32_t m_id = 0;
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

	template <typename T>
	static void pushResource(const T & res) {
		Resource::Ptr r = std::make_shared<T>(res);
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

