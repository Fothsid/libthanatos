#pragma once
#include "OBObject.h"

struct OBWeight
{
	uint32_t id;
	float weight;
};

class OBWeightList : public OBObject
{
public:
	OBWeightList();
	virtual ~OBWeightList();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	std::vector<std::vector<OBWeight>> list;
};