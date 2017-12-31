#pragma once

#include <cstddef>
#include <cassert>
#include <vector>

// template <typename C>
// class ComponentIterator : public std::iterator<std::input_iterator_tag, Entity::Id> {
// public:

// 	CType &operator ++() {
// 		_i++;
// 	}

// 	ComponentIterator(EntityManage *em, uint32_t index) :
// 		_em(em),
// 		_i(index),
// 		_capacity(em->capacity) {}
		
// 	bool operator == (const Delegate& rhs) const { return i_ == rhs.i_; }
// 	bool operator != (const Delegate& rhs) const { return i_ != rhs.i_; }
// 	ComponentHandle<C> operator * () { return ComponentEntity(manager_, manager_->create_id(i_)); }
// 	const ComponentHandle<C> operator * () const { return Entity(manager_, manager_->create_id(i_)); }

// protected:
	
// 	EntityManager *_em;
// 	uint32_t _i;
// };

class BasePool {

public:
	BasePool(std::size_t comp_size, std::size_t chunk_size = 8192) :
		_comp_size(comp_size), _chunk_size(chunk_size), _size(0), _capacity(0) {}

	std::size_t size() const { return _size; }
	std::size_t comp_size() const { return _comp_size; }
	std::size_t capacity() const { return _capacity; }

	/// Ensure at least n elements will fit in the pool.
	inline void expand(std::size_t n) {
		if (n >= _size) {
			if (n >= _capacity) reserve(n);
			_size = n;
		}		
	}
	
	inline void reserve(std::size_t n) {
		while(_capacity < n) {
			char *chunk = new char[_comp_size * _chunk_size];
			_buffer.push_back(chunk);
			_capacity += _chunk_size;
		}
	}

	inline void *get(std::size_t n) {
		assert(n < _size);
		return _buffer[n / _chunk_size] + (n % _chunk_size) * _comp_size;
	}

	virtual void destroy(std::size_t n) = 0;	
	
protected:

	std::size_t _comp_size;
	std::size_t _chunk_size;
	std::size_t _size;     //num elements stored
	std::size_t _capacity; //num elements booked already

	std::vector<char*> _buffer;
};


template <typename T>
class Pool : public BasePool {

public:
	
	Pool() : BasePool(sizeof(T)) {}

	virtual ~Pool() {
		// Component destructors *must* be called by owner.
	}
	
	void destroy(std::size_t n) {
		assert(n < _size);
		T *ptr = static_cast<T*>(get(n));
		ptr->~T();
	}

public:

//	std::vector<T*> _components;
//	std::size_t _num_components;
	
};
