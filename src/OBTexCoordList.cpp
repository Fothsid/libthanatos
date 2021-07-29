#include <cstdint>
#include <thanatos/OBTexCoordList.h>

OBTexCoordList::OBTexCoordList()
{
}

OBTexCoordList::~OBTexCoordList()
{
}

void OBTexCoordList::write(std::ostream& stream)
{
	if (list.size() > 0)
	{
		OBChkHeader header = { OBType::TexCoordList, 
							   list.size(), 
							   getSize() };
		stream.write((char*)&header, sizeof(OBChkHeader));
		stream.write((char*)&list[0], sizeof(OBTexCoord) * list.size());
	}
}

int OBTexCoordList::read(std::istream& stream)
{
	OBChkHeader header;
	stream.read((char*)&header, sizeof(OBChkHeader));

	if (header.type != OBType::TexCoordList)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBTexCoordList] Unexpected chunk type encountered. (%X)\n", header.type);
		return 0;
	}

	if ((header.size - 12) / header.count != sizeof(OBTexCoord))
	{
		fprintf(OB_ERROR_OUTPUT, "[OBTexCoordList] Incorrect size. (%X)\n", header.size);
		return 0;
	}

	list.resize(header.count);
	stream.read((char*)&list[0], sizeof(OBTexCoord) * header.count);

	return 1;
}

uint32_t OBTexCoordList::getSize()
{
	return list.size() * sizeof(OBTexCoord) + 12;
}

uint32_t OBTexCoordList::getType()
{
	return OBType::TexCoordList;
}