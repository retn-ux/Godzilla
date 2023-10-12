#ifndef _GODZILLA_BASE_GHANDLE_H_
#define _GODZILLA_BASE_GHANDLE_H_


#include "godzilla.h"
#include "common/gsharer.h"

#ifdef _WINDOWS_SYSTEM_
#include <Windows.h>

typedef HANDLE pins_t;

#define INVALID_PINS nullptr
#define CLOSE_PINS(hd) CloseHandle(hd)
#endif

#ifdef _LINUX_SYSTEM_

typedef pid_t pins_t;

#define INVALID_PINS 0
#define CLOSE_PINS(hd) 

#endif

class GHandle:
	public GSharer<pins_t>
{
public:
	GHandle() :GSharer() {
		element_ = INVALID_PINS;
	}
	GHandle(const pins_t& ins) :GSharer(ins){
		elem_release_func_ = ReleaseElement;
	}
	GHandle(pins_t&& ins) :GSharer(std::forward<pins_t>(ins)) {
		elem_release_func_ = ReleaseElement;
	}
	~GHandle() {}
public:
	bool IsInvalid() {
		return element_ == INVALID_PINS;
	}
private:
	static void ReleaseElement(pins_t& element) {
		if (element != INVALID_PINS) {
			CLOSE_PINS(element);
		}
	}
};

#endif