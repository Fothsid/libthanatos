#include <cstdint>
#include <thanatos/OBJointRefList.h>

OBJointRefList::OBJointRefList()
{
}

OBJointRefList::~OBJointRefList()
{
}

void OBJointRefList::write(std::ostream& stream)
{
	if (list.size() > 0)
	{
		OBChkHeader header = { OBType::JointRefList, 
			                   list.size(), 
			                   list.size() * sizeof(int32_t) + 12 };
		stream.write((char*)&header, sizeof(OBChkHeader));
		stream.write((char*)&list[0], sizeof(int32_t) * list.size());
	}
}

int OBJointRefList::read(std::istream& stream)
{
	OBChkHeader header;
	stream.read((char*)&header, sizeof(OBChkHeader));

	if (header.type != OBType::JointRefList)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBJointRefList] Unexpected chunk type encountered. (%X)\n", header.type);
		return 0;
	}

	if ((header.size - 12) / header.count != sizeof(int32_t))
	{
		fprintf(OB_ERROR_OUTPUT, "[OBJointRefList] Wrong size\n");
		return 0;
	}

	list.resize(header.count);
	stream.read((char*)&list[0], sizeof(int32_t) * header.count);

	return 1;
}

uint32_t OBJointRefList::getSize()
{
	return list.size() * sizeof(int32_t) + 12;
}

uint32_t OBJointRefList::getType()
{
	return OBType::JointRefList;
}