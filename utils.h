#pragma once
#ifndef _NC_UTILS_H_
#define _NC_UTILS_H_

#include <iostream>

inline void Log() { std::cout << std::endl; }

inline void DebugLog() { std::cout << std::endl; }

template< typename First, typename ... Rest>
inline void DebugLog(First&& first, Rest&& ... rest)
{
#ifdef _DEBUG
	std::cout << std::forward<First>(first);
	DebugLog(std::forward<Rest>(rest)...);
#endif // DEBUG
}

template< typename First, typename ... Rest>
inline void Log(First&& first, Rest&& ... rest)
{
	std::cout << std::forward<First>(first);
	Log(std::forward<Rest>(rest)...);
}

#endif // !_NC_UTILS_H_
