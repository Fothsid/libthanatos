#pragma once
#include "OBObject.h"

struct OBColor
{
	float r, g, b, a;
};

inline bool operator== (const OBColor& l, const OBColor& r)
{
	return l.r == r.r && l.g == r.g && l.b == r.b && l.a == r.a;
}

class OBColorList : public OBObject
{
public:
	OBColorList();
	virtual ~OBColorList();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	std::vector<OBColor> list;
};