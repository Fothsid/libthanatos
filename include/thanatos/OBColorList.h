#pragma once
#include "OBObject.h"

struct OBColor
{
	float r, g, b, a;
};

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