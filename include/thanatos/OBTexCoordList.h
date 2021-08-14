#pragma once
#include "OBObject.h"

struct OBTexCoord
{
	float u, v;
};

inline bool operator== (const OBTexCoord& l, const OBTexCoord& r)
{
	return l.u == r.u && l.v == r.v;
}

class OBTexCoordList : public OBObject
{
public:
	OBTexCoordList();
	virtual ~OBTexCoordList();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	std::vector<OBTexCoord> list;
};