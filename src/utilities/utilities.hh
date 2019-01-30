#pragma once

#include <windows.h>
#include <psapi.h>
#include <chrono>
#include <thread>

#define IN_RANGE(x, a, b) (x >= a && x <= b)
#define GET_BITS(x)       (IN_RANGE((x & ~0x20), 'A', 'F') ? ((x & ~0x20) - 'A' + 0xa) : (IN_RANGE(x, '0', '9') ? x - '0' : 0))
#define GET_BYTE(x)       (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))

namespace utilities {

	template <class T>
	constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
		return (v >= lo && v <= hi) ? v : (v < lo ? lo : hi);
	}

	template <class T>
	constexpr const T& minimum( const T& x, const T& y )
	{
		return ( x > y ) ? y : x;
	}

	template <class T>
	constexpr const T& maximum( const T& x, const T& y )
	{
		return ( x < y ) ? y : x;
	}

    template <typename type>
    void wait(type duration) {
        std::this_thread::sleep_for(duration);
    }

	bool can_shoot(bool right_click);
	float latency();
	const wchar_t* to_wchar(const char* string);
    uintptr_t find_occurrence(const std::string &module_name, const std::string &pattern);
	float curtime();
	unsigned int find_pattern(const char* module_name, const BYTE* mask, const char* mask_string);
}