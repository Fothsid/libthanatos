#pragma once
#include "OBObject.h"
#include "OBModel.h"
#include "OBHierarchy.h"

struct OBNbdTexture
{
	uint32_t size;
	void* tim2Data;
};

class OBNbd : public OBObject
{
public:
	OBNbd();
	virtual ~OBNbd();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	std::vector<OBNbdTexture> textures;
	OBModel amo;
	OBHierarchy ahi;

};