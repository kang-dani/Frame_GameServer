#pragma once
#include "PlayerIdGenerator.h"

uint32 PlayerIdGenerator::Generator()
{
	static std::atomic<uint32> idCounter = 1;
	return fetch_add(1);
}
