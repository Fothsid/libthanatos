#pragma once
#include "OBObject.h"

class OBJointRefList : public OBObject
{
public:
	OBJointRefList();
	virtual ~OBJointRefList();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	std::vector<int32_t> list;
};