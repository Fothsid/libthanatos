#pragma once
#include "OBObject.h"
#include "OBDate.h"
#include "OBMeshList.h"
#include "OBMaterialList.h"
#include "OBTextureList.h"

class OBModel : public OBObject
{
public:
	OBModel();
	virtual ~OBModel();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	int getSubchunkCount();

	OBDate* date                 = 0;
	OBMeshList* meshList         = 0;
	OBMaterialList* materialList = 0;
	OBTextureList* textureList   = 0;
};