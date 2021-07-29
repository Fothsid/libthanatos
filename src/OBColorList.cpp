#include <cstdint>
#include <thanatos/OBColorList.h>

OBColorList::OBColorList()
{
}

OBColorList::~OBColorList()
{
}

void OBColorList::write(std::ostream& stream)
{
	if (list.size() > 0)
	{
		OBChkHeader header = { OBType::ColorList, 
							   list.size(), 
							   list.size() * sizeof(OBColor) + 12 };
		stream.write((char*)&header, sizeof(OBChkHeader));
		stream.write((char*)&list[0], sizeof(OBColor) * list.size());
	}
}

int OBColorList::read(std::istream& stream)
{
	OBChkHeader header;
	stream.read((char*)&header, sizeof(OBChkHeader));

	if (header.type != OBType::ColorList)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBColorList] Unexpected chunk type encountered. (%X)\n", header.type);
		return 0;
	}

	if ((header.size - 12) / header.count != sizeof(OBColor))
	{
		fprintf(OB_ERROR_OUTPUT, "[OBColorList] Wrong size\n");
		return 0;
	}

	list.resize(header.count);
	stream.read((char*)&list[0], sizeof(OBColor) * header.count);

	return 1;
}

uint32_t OBColorList::getSize()
{
	return list.size() * sizeof(OBColor) + 12;
}

uint32_t OBColorList::getType()
{
	return OBType::ColorList;
}