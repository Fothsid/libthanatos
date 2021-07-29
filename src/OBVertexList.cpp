#include <cstdint>
#include <thanatos/OBVertexList.h>

OBVertexList::OBVertexList()
{
}

OBVertexList::~OBVertexList()
{
}

void OBVertexList::write(std::ostream& stream)
{
	if (list.size() > 0)
	{
		OBChkHeader header = { OBType::VertexList, 
							   list.size(), 
							   list.size() * sizeof(OBVertex) + 12 };
		stream.write((char*)&header, sizeof(OBChkHeader));
		stream.write((char*)&list[0], sizeof(OBVertex) * list.size());
	}
}

int OBVertexList::read(std::istream& stream)
{
	OBChkHeader header;
	stream.read((char*)&header, sizeof(OBChkHeader));

	if (header.type != OBType::VertexList)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBVertexList] Unexpected chunk type encountered. (%X)\n", header.type);
		return 0;
	}

	if ((header.size - 12) / header.count != sizeof(OBVertex))
	{
		fprintf(OB_ERROR_OUTPUT, "[OBVertexList] Incorrect size. (%X)\n", header.size);
		return 0;
	}

	list.resize(header.count);
	stream.read((char*)&list[0], sizeof(OBVertex) * header.count);

	return 1;
}

uint32_t OBVertexList::getSize()
{
	return list.size() * sizeof(OBVertex) + 12;
}

uint32_t OBVertexList::getType()
{
	return OBType::VertexList;
}