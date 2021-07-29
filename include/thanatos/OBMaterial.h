#pragma once
#include "OBObject.h"
#include "OBColorList.h"

class OBMaterial : public OBObject
{
public:
	OBMaterial();
	virtual ~OBMaterial();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	uint32_t type;
	OBColor ambient;
	OBColor diffuse;
	OBColor specular;
	float specularDecay;
	std::vector<uint32_t> textures;
};