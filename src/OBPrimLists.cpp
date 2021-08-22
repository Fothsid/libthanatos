#include <cstdint>
#include <thanatos/OBPrimLists.h>
#include <thanatos/OBTStripList.h>

OBPrimLists::OBPrimLists()
{
}

OBPrimLists::~OBPrimLists()
{
}

void OBPrimLists::write(std::ostream& stream)
{
	if (lists.size() > 0)
	{
		int listCount = 0;
		if (lists[0].list.size() > 0)
			listCount++;
		if (lists.size() > 1 && lists[1].list.size() > 0)
			listCount++;

		OBChkHeader header = { OBType::PrimLists,
							   listCount, 
							   getSize() };
		stream.write((char*)&header, sizeof(OBChkHeader));
		for (int i = 0; i < lists.size(); i++)
			lists[i].write(stream);
	}
}

int OBPrimLists::read(std::istream& stream)
{
	OBChkHeader header;
	stream.read((char*)&header, sizeof(OBChkHeader));

	if (header.type != OBType::PrimLists)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBPrimLists] Unexpected chunk type encountered. (%X)\n", header.type);
		return 0;
	}

	if (header.count > 2)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBPrimLists] More than 2 triangle strip lists.\n");
		return 0;
	}

	lists.resize(header.count);
	for (int i = 0; i < header.count; i++)
		if (!lists[i].read(stream))
		{
			fprintf(OB_ERROR_OUTPUT, "[OBPrimLists] Unable to read a triangle strip list. (%d)\n", i);
			return 0;
		}

	return 1;
}

uint32_t OBPrimLists::getSize()
{
	uint32_t size = 12;
	for (int i = 0; i < lists.size(); i++)
		size += lists[i].getSize();
	return size;
}

uint32_t OBPrimLists::getType()
{
	return OBType::PrimLists;
}