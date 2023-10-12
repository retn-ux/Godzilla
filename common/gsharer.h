#ifndef _GODZILLA_COMMON_GSHARER_H_
#define _GODZILLA_COMMON_GSHARER_H_
#include <xmemory>

#include "godzilla.h"

template<typename _Ty>
class GSharer {
public:
	GSharer() :counter_(nullptr) {}

	GSharer(const _Ty& element) :
		counter_(nullptr), 
		elem_release_func_(nullptr)
	{
		Create(element);
	}

	GSharer(_Ty&& element) :
		counter_(nullptr), 
		elem_release_func_(nullptr) 
	{
		Create(std::forward<_Ty>(element));
	}

	GSharer(const GSharer& other) :
		counter_(nullptr), 
		elem_release_func_(nullptr) 
	{
		Create(other);
	}

	GSharer(GSharer&& other) :
		counter_(nullptr), 
		elem_release_func_(nullptr) 
	{
		Create(std::forward<GSharer>(other));
	}

	virtual ~GSharer() {
		Destory();
	}
public:
	void Reset(const _Ty& element) {
		Destory();
		Create(element);
	}

	void Reset(_Ty&& element) {
		Destory();
		Create(std::forward<_Ty>(element));
	}

	void Reset(const GSharer& other) {
		Destory();
		Create(other);
	}

	void Reset(GSharer&& other) {
		Destory();
		Create(std::forward<GSharer>(other));
	}

	_Ty& GetElement() {
		return element_;
	}
public:
	void operator=(const _Ty& element) {
		Reset(element);
	}

	void operator=(_Ty&& element) {
		Reset(std::forward<_Ty>(element));
	}

	void operator=(const GSharer& other) {
		Reset(other);
	}

	void operator=(GSharer&& other) {
		Reset(std::forward<GSharer>(other));
	}
protected:
	typedef void (*ReleaseElementFunc)(_Ty& element);
	ReleaseElementFunc elem_release_func_;
	_Ty	element_;
private:
	void Destory() {
		if (counter_ != nullptr) {
			(*counter_)--;
			if (*counter_ == 0) {
				alloc_.destroy(counter_);
				counter_ = nullptr;
				if (elem_release_func_ != nullptr) {
					elem_release_func_(element_);
				}
			}
		}
	}

	void Create(const _Ty& element) {
		counter_ = alloc_.allocate(1);
		*counter_ = 1;
		element_ = element;
	}

	void Create(_Ty&& element) {
		counter_ = alloc_.allocate(1);
		*counter_ = 1;
		element_ = std::forward<_Ty>(element);
	}

	void Create(const GSharer& other) {
		counter_ = other.counter_;
		element_ = other.element_;
		elem_release_func_ = other.elem_release_func_;
		(*counter_)++;
	}

	void Create(GSharer&& other) {
		counter_ = other.counter_;
		element_ = other.element_;
		elem_release_func_ = other.elem_release_func_;
		other.Destory();
	}
private:
	std::allocator<uint32_t> alloc_;
	uint32_t* counter_;
};

#endif