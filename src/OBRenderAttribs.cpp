#include <cstdint>
#include <thanatos/OBRenderAttribs.h>

OBRenderAttribs::OBRenderAttribs()
{
}

OBRenderAttribs::~OBRenderAttribs()
{
}

void OBRenderAttribs::write(std::ostream& stream)
{
	OBChkHeader header = { OBType::RenderAttribs, 
		                   1, 
		                   sizeof(_OBRenderAttribs) + 12 };
	stream.write((char*)&header, sizeof(OBChkHeader));
	stream.write((char*)&attribs, sizeof(_OBRenderAttribs));
}

int OBRenderAttribs::read(std::istream& stream)
{
	OBChkHeader header;
	stream.read((char*)&header, sizeof(OBChkHeader));

	if (header.type != OBType::RenderAttribs)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBRenderAttribs] Unexpected chunk type encountered. (%X)\n", header.type);
		return 0;
	}

	if (header.count != 1)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBDate] Incorrect count. (%X)\n", header.count);
		return 0;
	}

	if ((header.size - 12) / header.count != sizeof(_OBRenderAttribs))
	{
		fprintf(OB_ERROR_OUTPUT, "[OBDate] Incorrect size. (%X)\n", header.size);
		return 0;
	}

	stream.read((char*)&attribs, sizeof(_OBRenderAttribs));

	return 1;
}

uint32_t OBRenderAttribs::getSize()
{
	return sizeof(_OBRenderAttribs) + 12;
}

uint32_t OBRenderAttribs::getType()
{
	return OBType::RenderAttribs;
}