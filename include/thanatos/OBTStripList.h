#pragma once
#include "OBObject.h"

struct OBTStrip
{
	bool leftHand;
	std::vector<uint32_t> indices;
};

class OBTStripList : public OBObject
{
public:
	OBTStripList();
	virtual ~OBTStripList();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	int type;
	std::vector<OBTStrip> list;
};