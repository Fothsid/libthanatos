#include <cstdint>
#include <thanatos/OBTStripMaterialList.h>

OBTStripMaterialList::OBTStripMaterialList()
{
}

OBTStripMaterialList::~OBTStripMaterialList()
{
}

void OBTStripMaterialList::write(std::ostream& stream)
{
	if (list.size() > 0)
	{
		OBChkHeader header = { OBType::TStripMaterialList, 
			                   list.size(), 
			                   list.size() * sizeof(uint32_t) + 12 };
		stream.write((char*)&header, sizeof(OBChkHeader));
		stream.write((char*)&list[0], sizeof(uint32_t) * list.size());
	}
}

int OBTStripMaterialList::read(std::istream& stream)
{
	OBChkHeader header;
	stream.read((char*)&header, sizeof(OBChkHeader));

	if (header.type != OBType::TStripMaterialList)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBTStripMaterialList] Unexpected chunk type encountered. (%X)\n", header.type);
		return 0;
	}

	if ((header.size - 12) / header.count != sizeof(int32_t))
	{
		fprintf(OB_ERROR_OUTPUT, "[OBTStripMaterialList] Incorrect size. (%X)\n", header.size);
		return 0;
	}

	list.resize(header.count);
	stream.read((char*)&list[0], sizeof(int32_t) * header.count);

	return 1;
}

uint32_t OBTStripMaterialList::getSize()
{
	return list.size() * sizeof(int32_t) + 12;
}

uint32_t OBTStripMaterialList::getType()
{
	return OBType::TStripMaterialList;
}