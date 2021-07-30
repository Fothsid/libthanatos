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
		for (int i = 0; i < list.size(); i++)
		{
			float rgba[4];
			rgba[0] = list[i].r * 255.0f;
			rgba[1] = list[i].g * 255.0f;
			rgba[2] = list[i].b * 255.0f;
			rgba[3] = list[i].a * 255.0f;
			stream.write((char*)&rgba[0], sizeof(OBColor));
		}
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

	/* 
		Converting color vertices from 0-255 range to 0-1 range because
		0-255 range in floats is ugly
	 */

	for (int i = 0; i < list.size(); i++)
	{
		list[i].r /= 255.0f;
		list[i].g /= 255.0f;
		list[i].b /= 255.0f;
		list[i].a /= 255.0f;
	}

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