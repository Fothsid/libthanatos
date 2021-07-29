#pragma once
#include "OBObject.h"
#include "OBTStripList.h"

class OBPrimLists : public OBObject
{
public:
	OBPrimLists();
	virtual ~OBPrimLists();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	std::vector<OBTStripList> lists;
};