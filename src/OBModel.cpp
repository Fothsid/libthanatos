#include <cstdint>
#include <thanatos/OBModel.h>

OBModel::OBModel()
{
}

OBModel::~OBModel()
{
	if (date)         delete date;
	if (meshList)     delete meshList;
	if (materialList) delete materialList;
	if (textureList)  delete textureList;
}

void OBModel::write(std::ostream& stream)
{
	OBChkHeader header = { OBType::Model,
		                   getSubchunkCount(), 
		                   getSize() };
	stream.write((char*)&header, sizeof(OBChkHeader));
	if (date)         date->write(stream);
	if (meshList)     meshList->write(stream);
	if (materialList) materialList->write(stream);
	if (textureList)  textureList->write(stream);
}

int OBModel::read(std::istream& stream)
{
	OBChkHeader header;
	stream.read((char*)&header, sizeof(OBChkHeader));

	if (header.type != OBType::Model)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBModel] Unexpected chunk type encountered. (%X)\n", header.type);
		return 0;
	}

	for (uint32_t i = 0; i < header.count; i++)
	{
		OBChkHeader cHead;
		
		int len = stream.tellg();
		stream.read((char*)&cHead, sizeof(OBChkHeader));
		stream.seekg(len, std::ios_base::beg);

		OBObject* child = NULL;
		bool unexpected = false;
		switch (cHead.type)
		{
			case OBType::Date:         date         = new OBDate();         child = (OBObject*) date; break;
			case OBType::MeshList:     meshList     = new OBMeshList();     child = (OBObject*) meshList; break;
			case OBType::MaterialList: materialList = new OBMaterialList(); child = (OBObject*) materialList; break;
			case OBType::TextureList:  textureList  = new OBTextureList();  child = (OBObject*) textureList; break;
			default: unexpected = true; break;
		}
		if (!child)
		{
			if (unexpected)
				fprintf(OB_ERROR_OUTPUT, "[OBModel] Unexpected chunk: %X (pos:%X)\n", cHead.type, stream.tellg());
			else
				fprintf(OB_ERROR_OUTPUT, "[OBModel] Could not allocate memory for chunk type %X\n", cHead.type);
			return 0;
		}
		if (!child->read(stream))
		{
			fprintf(OB_ERROR_OUTPUT, "[OBModel] Error while reading: %X (pos:%X)\n", cHead.type, len);
			return 0;
		}
	}

	return 1;
}

uint32_t OBModel::getSize()
{
	uint32_t size = 12;
	if (date)         size += date->getSize();
	if (meshList)     size += meshList->getSize();
	if (materialList) size += materialList->getSize();
	if (textureList)  size += textureList->getSize();
	return size;
}

uint32_t OBModel::getType()
{
	return OBType::Model;
}

int OBModel::getSubchunkCount()
{
	int count = 0;
	if (date)         count++;
	if (meshList)     count++;
	if (materialList) count++;
	if (textureList)  count++;
	return count;
}