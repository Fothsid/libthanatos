#include <cstdint>
#include <thanatos/OBNormalList.h>

OBNormalList::OBNormalList()
{
}

OBNormalList::~OBNormalList()
{
}

void OBNormalList::write(std::ostream& stream)
{
	if (list.size() > 0)
	{
		OBChkHeader header = { OBType::NormalList, 
							   list.size(), 
							   list.size() * sizeof(OBNormal) + 12 };
		stream.write((char*)&header, sizeof(OBChkHeader));
		stream.write((char*)&list[0], sizeof(OBNormal) * list.size());
	}
}

int OBNormalList::read(std::istream& stream)
{
	OBChkHeader header;
	stream.read((char*)&header, sizeof(OBChkHeader));

	if (header.type != OBType::NormalList)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBNormalList] Unexpected chunk type encountered. (%X)\n", header.type);
		return 0;
	}

	if ((header.size - 12) / header.count != sizeof(OBNormal))
	{
		fprintf(OB_ERROR_OUTPUT, "[OBNormalList] Incorrect size.\n");
		return 0;
	}

	list.resize(header.count);
	stream.read((char*)&list[0], sizeof(OBNormal) * header.count);

	return 1;
}

uint32_t OBNormalList::getSize()
{
	return list.size() * sizeof(OBNormal) + 12;
}

uint32_t OBNormalList::getType()
{
	return OBType::NormalList;
}