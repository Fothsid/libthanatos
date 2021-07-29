#include <cstdint>
#include <cstring>
#include <thanatos/OBTextureList.h>

struct _OBAMOTextureRef
{
	OBChkHeader header;
	int32_t id;
	int32_t width;
	int32_t height;
	int32_t tiling;
	int32_t dummy0[60];
};

OBTextureList::OBTextureList()
{
}

OBTextureList::~OBTextureList()
{
}

void OBTextureList::write(std::ostream& stream)
{
	if (list.size() > 0)
	{
		OBChkHeader header = { OBType::TextureList, 
							   list.size(), 
							   list.size() * sizeof(_OBAMOTextureRef) + 12 };
		stream.write((char*)&header, sizeof(OBChkHeader));

		for (int i = 0; i < list.size(); i++)
		{
			_OBAMOTextureRef ref;
			memset(&ref, 0, sizeof(_OBAMOTextureRef));
			ref.header.type = OBType::TextureRef;
			ref.header.size = sizeof(_OBAMOTextureRef);
			ref.header.count = 1;
			ref.id = list[i].id;
			ref.width = list[i].width;
			ref.height = list[i].height;
			ref.tiling = list[i].tiling;
			stream.write((char*)&ref, sizeof(_OBAMOTextureRef));
		}
	}
}

int OBTextureList::read(std::istream& stream)
{
	OBChkHeader header;
	stream.read((char*)&header, sizeof(OBChkHeader));

	if (header.type != OBType::TextureList)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBTextureList] Unexpected chunk type encountered. (%X)\n", header.type);
		return 0;
	}

	if ((header.size - 12) / header.count != sizeof(_OBAMOTextureRef))
	{
		fprintf(OB_ERROR_OUTPUT, "[OBTextureList] Incorrect size. (%X)\n", header.size);
		return 0;
	}

	list.resize(header.count);
	for (int i = 0; i < header.count; i++)
	{
		_OBAMOTextureRef ref;
		stream.read((char*)&ref, sizeof(_OBAMOTextureRef));

		if (ref.header.type != OBType::TextureRef ||
			ref.header.size != sizeof(_OBAMOTextureRef))
		{
			fprintf(OB_ERROR_OUTPUT, "[OBTextureList] Unexpected chunk type for texture reference encountered. (%X)\n", ref.header.type);
			return 0;
		}

		list[i].id = ref.id;
		list[i].width = ref.width;
		list[i].height = ref.height;
		list[i].tiling = ref.tiling;
	}

	return 1;
}

uint32_t OBTextureList::getSize()
{
	return list.size() * sizeof(_OBAMOTextureRef) + 12;
}

uint32_t OBTextureList::getType()
{
	return OBType::TextureList;
}