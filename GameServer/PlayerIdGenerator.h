#pragma once
#include "Struct.pb.h"
#include <cstdint>  // uint32_t
#include <atomic>

using uint32 = std::uint32_t;

class PlayerIdGenerator
{
public:
	static uint32 Generator()
	{
		static std::atomic<uint32> idCounter = 1;
		return idCounter.fetch_add(1);
	}
};
