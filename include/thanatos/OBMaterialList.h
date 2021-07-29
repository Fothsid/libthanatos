#pragma once
#include "OBObject.h"
#include "OBMaterial.h"

class OBMaterialList : public OBObject
{
public:
	OBMaterialList();
	virtual ~OBMaterialList();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	std::vector<OBMaterial> list;
};