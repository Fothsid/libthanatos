#pragma once
#include <thanatos/OBTypes.h>

#define OBTYPE_DECL(name, value) \
	case (value): return #name; \

const char* OBType::getName(uint32_t type)
{
	switch (type)
	{
		OBTYPE_LIST
		default: return "Unknown";
	}
}
#undef OBTYPE_DECL