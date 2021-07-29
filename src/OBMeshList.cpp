#include <cstdint>
#include <thanatos/OBMeshList.h>

OBMeshList::OBMeshList()
{
}

OBMeshList::~OBMeshList()
{
}

void OBMeshList::write(std::ostream& stream)
{
	if (list.size() > 0)
	{
		OBChkHeader header = { OBType::MeshList,
			                   list.size(),
			                   getSize() };;
		stream.write((char*)&header, sizeof(OBChkHeader));
		for (int i = 0; i < list.size(); i++)
			list[i].write(stream);
	}
}

int OBMeshList::read(std::istream& stream)
{
	OBChkHeader header;
	stream.read((char*)&header, sizeof(OBChkHeader));

	if (header.type != OBType::MeshList)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBMeshList] Unexpected chunk type encountered. (%X)\n", header.type);
		return 0;
	}

	list.resize(header.count);
	for (int i = 0; i < header.count; i++)
		if (!list[i].read(stream))
		{
			fprintf(OB_ERROR_OUTPUT, "[OBMeshList] Error while reading OBMesh\n");
			return 0;
		}

	return 1;
}

uint32_t OBMeshList::getSize()
{
	uint32_t size = 12;
	for (int i = 0; i < list.size(); i++)
		size += list[i].getSize();
	return size;
}

uint32_t OBMeshList::getType()
{
	return OBType::MeshList;
}