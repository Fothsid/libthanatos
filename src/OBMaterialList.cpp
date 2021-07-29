#include <cstdint>
#include <thanatos/OBMaterialList.h>

OBMaterialList::OBMaterialList()
{
}

OBMaterialList::~OBMaterialList()
{
}

void OBMaterialList::write(std::ostream& stream)
{
	if (list.size() > 0)
	{
		OBChkHeader header = { OBType::MaterialList,
							   list.size(), 
							   getSize() };;
		stream.write((char*)&header, sizeof(OBChkHeader));
		for (int i = 0; i < list.size(); i++)
			list[i].write(stream);
	}
}

int OBMaterialList::read(std::istream& stream)
{
	OBChkHeader header;
	stream.read((char*)&header, sizeof(OBChkHeader));

	if (header.type != OBType::MaterialList)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBMaterialList] Unexpected chunk type encountered. (%X)\n", header.type);
		return 0;
	}

	list.resize(header.count);
	for (uint32_t i = 0; i < header.count; i++)
		if (!list[i].read(stream))
		{
			fprintf(OB_ERROR_OUTPUT, "[OBMaterialList] Couldn't read a material.\n");
			return 0;
		}

	return 1;
}

uint32_t OBMaterialList::getSize()
{
	return list.size() * (list.size() > 0 ? list[0].getSize() : 0) + 12;
}

uint32_t OBMaterialList::getType()
{
	return OBType::MaterialList;
}