#include <cstdint>
#include <thanatos/OBWeightList.h>

OBWeightList::OBWeightList()
{
}

OBWeightList::~OBWeightList()
{
}

void OBWeightList::write(std::ostream& stream)
{
	if (list.size() > 0)
	{
		OBChkHeader header = { OBType::WeightList, 
							   list.size(), 
							   getSize() };
		stream.write((char*)&header, sizeof(OBChkHeader));
		for (uint32_t i = 0; i < list.size(); i++)
		{
			uint32_t jointCount = list[i].size();
			stream.write((char*)&jointCount, sizeof(uint32_t));
			stream.write((char*)&list[i][0], sizeof(OBWeight) * jointCount);
		}
	}
}

int OBWeightList::read(std::istream& stream)
{
	OBChkHeader header;
	stream.read((char*)&header, sizeof(OBChkHeader));

	if (header.type != OBType::WeightList)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBWeightList] Unexpected chunk type encountered. (%X)\n", header.type);
		return 0;
	}

	if ((header.size - 12) / header.count < sizeof(OBWeight))
	{
		fprintf(OB_ERROR_OUTPUT, "[OBWeightList] Incorrect size. (%X)\n", header.type);
		return 0;
	}

	list.resize(header.count);
	for (uint32_t i = 0; i < header.count; i++)
	{
		uint32_t jointCount = 0;
		stream.read((char*)&jointCount, sizeof(uint32_t));
		list[i].resize(jointCount);
		stream.read((char*)&list[i][0], sizeof(OBWeight) * jointCount);
	}

	return 1;
}

uint32_t OBWeightList::getSize()
{
	uint32_t size = list.size() * sizeof(uint32_t) + 12;
	for (int i = 0; i < list.size(); i++)
		size += list[i].size() * sizeof(OBWeight);
	return size;
}

uint32_t OBWeightList::getType()
{
	return OBType::WeightList;
}