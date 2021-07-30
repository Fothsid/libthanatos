#pragma once
#include <cstdint>

namespace OBSld
{
	uint32_t decompress(void* src, uint32_t srcSize, void* dest, uint32_t destSize);
	uint32_t compress(void* src, uint32_t srcSize, void* dst, uint32_t destSize);
};