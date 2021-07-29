#pragma once
#include <cstdint>
#include <ostream>
#include <istream>
#include <vector>
#include <string>
#include <cstdio>
#include <thanatos/OBTypes.h>

struct OBChkHeader
{
	uint32_t type;
	uint32_t count;
	uint32_t size;
};

class OBObject
{
public:
	OBObject();
	virtual ~OBObject();
	virtual void write(std::ostream& stream) = 0;
	virtual int read(std::istream& stream) = 0;
	virtual uint32_t getSize() = 0;
	virtual uint32_t getType() = 0;

	template<typename T>
	T* as() { return (T*) this; }
};