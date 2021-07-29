#include <cstdint>
#include <thanatos/OBMaterial.h>

struct _OBAMOMaterial
{
	OBColor ambient;
	OBColor diffuse;
	OBColor specular;
	float specularDecay;
	int32_t texCount;
	uint32_t _dummy0[50];
};

OBMaterial::OBMaterial()
{
}

OBMaterial::~OBMaterial()
{
}

void OBMaterial::write(std::ostream& stream)
{
	if (type)
	{
		OBChkHeader header = { type, 
			                   1,
			                   getSize() };
		stream.write((char*)&header, sizeof(OBChkHeader));
		
		_OBAMOMaterial mat = {0};
		mat.ambient = ambient;
		mat.diffuse = diffuse;
		mat.specular = specular;
		mat.specularDecay = specularDecay;
		mat.texCount = textures.size();

		stream.write((char*)&mat, sizeof(_OBAMOMaterial));
		stream.write((char*)&textures[0], sizeof(uint32_t) * textures.size());
	}
}

int OBMaterial::read(std::istream& stream)
{
	OBChkHeader header;
	stream.read((char*)&header, sizeof(OBChkHeader));

	if (header.type != OBType::MaterialConstant &&
		header.type != OBType::MaterialLambert &&
		header.type != OBType::MaterialPhong &&
		header.type != OBType::MaterialVertColors)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBMaterial] Unknown material type: %X\n", header.type);
		return 0;
	}

	_OBAMOMaterial mat;
	stream.read((char*)&mat, sizeof(_OBAMOMaterial));

	if ((header.size - 12) != sizeof(_OBAMOMaterial) + (mat.texCount * sizeof(uint32_t)))
	{
		fprintf(OB_ERROR_OUTPUT, "[OBMaterial] Incorrect material chunk size: %X (pos:%X)\n", header.size, stream.tellg());
		return 0;
	}

	type = header.type;
	ambient = mat.ambient;
	diffuse = mat.diffuse;
	specular = mat.specular;
	specularDecay = mat.specularDecay;

	textures.resize(mat.texCount);
	stream.read((char*)&textures[0], sizeof(uint32_t) * mat.texCount);

	return 1;
}

uint32_t OBMaterial::getSize()
{
	return sizeof(_OBAMOMaterial) + (textures.size() * sizeof(uint32_t)) + 12;
}

uint32_t OBMaterial::getType()
{
	return type;
}