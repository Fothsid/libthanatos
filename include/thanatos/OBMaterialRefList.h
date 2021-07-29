#pragma once
#include "OBObject.h"

class OBMaterialRefList : public OBObject
{
public:
	OBMaterialRefList();
	virtual ~OBMaterialRefList();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	std::vector<int32_t> list;
};