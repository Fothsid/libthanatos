#pragma once
#include "OBObject.h"

struct OBVertex
{
	float x, y, z;
};

inline bool operator== (const OBVertex& l, const OBVertex& r)
{
	return l.x == r.x && l.y == r.y && l.z == r.z;
}

class OBVertexList : public OBObject
{
public:
	OBVertexList();
	virtual ~OBVertexList();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	std::vector<OBVertex> list;
};