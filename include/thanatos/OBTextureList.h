#pragma once
#include "OBObject.h"

struct OBTextureRef
{
	int id;
	int width, height;
	int tiling;
};

class OBTextureList : public OBObject
{
public:
	OBTextureList();
	virtual ~OBTextureList();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	std::vector<OBTextureRef> list;
};