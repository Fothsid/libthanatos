#pragma once
#include "OBObject.h"
#include "OBMesh.h"

class OBMeshList : public OBObject
{
public:
	OBMeshList();
	virtual ~OBMeshList();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	std::vector<OBMesh> list;
};