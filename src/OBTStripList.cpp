#include <cstdint>
#include <thanatos/OBTStripList.h>

OBTStripList::OBTStripList()
{
}

OBTStripList::~OBTStripList()
{
}

void OBTStripList::write(std::ostream& stream)
{
	if (list.size() > 0)
	{
		OBChkHeader header = { type,
							   list.size(), 
							   getSize() };;
		stream.write((char*)&header, sizeof(OBChkHeader));
		for (uint32_t i = 0; i < list.size(); i++)
		{
			uint32_t indexCount = list[i].indices.size();
			if (list[i].leftHand)
				indexCount |= 0x80000000;
			stream.write((char*)&indexCount, sizeof(uint32_t));
			stream.write((char*)&list[i].indices[0], sizeof(uint32_t) * list[i].indices.size());
		}
	}
}

int OBTStripList::read(std::istream& stream)
{
	OBChkHeader header;
	stream.read((char*)&header, sizeof(OBChkHeader));

	if (header.type != OBType::TStripListFW &&
		header.type != OBType::TStripListPW)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBTStripList] Unexpected chunk type encountered. (%X)\n", header.type);
		return 0;
	}

	type = header.type;
	list.resize(header.count);
	for (uint32_t i = 0; i < header.count; i++)
	{
		uint32_t indexCount = 0;
		stream.read((char*)&indexCount, sizeof(uint32_t));
		list[i].leftHand = (indexCount & 0x80000000 == 0x80000000) ? true : false;
		
		indexCount &= 0x7FFFFFFF;
		list[i].indices.resize(indexCount);
		stream.read((char*)&list[i].indices[0], sizeof(uint32_t) * indexCount);
	}

	return 1;
}

uint32_t OBTStripList::getSize()
{
	uint32_t size = list.size() * sizeof(uint32_t) + 12;
	for (int i = 0; i < list.size(); i++)
		size += list[i].indices.size() * sizeof(uint32_t);
	return size;
}

uint32_t OBTStripList::getType()
{
	return type;
}