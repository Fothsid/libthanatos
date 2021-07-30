#include <cstdint>
#include <list>
#include <algorithm>
#include <thanatos/OBNbd.h>
#include <thanatos/OBSld.h>

struct NBDEntry
{
	uint32_t type;
	uint32_t offset;
	uint32_t size;
	uint32_t texCount;
};

struct NBDHeader
{
	NBDEntry tex, amo, ahi;
	NBDEntry dummy;
};

OBNbd::OBNbd()
{
}

OBNbd::~OBNbd()
{
	for (int i = 0; i < textures.size(); i++)
	{
		if (textures[i].tim2Data)
			free(textures[i].tim2Data);
	}
}

#define AlignTo16(X) (16 - (X) % 16)

void OBNbd::write(std::ostream& stream)
{
	NBDHeader header = {0};
	header.tex.type = 0x584554;
	header.amo.type = 0x4F4D41;
	header.ahi.type = 0x494841;
	
	std::vector<OBNbdTexture> cTextures(textures.size());
	header.tex.texCount = (uint32_t) textures.size();
	header.tex.size = 0;
	for (int i = 0; i < cTextures.size(); i++)
	{
		cTextures[i].tim2Data = malloc(textures[i].size * 2);
		cTextures[i].size = OBSld::compress(textures[i].tim2Data, textures[i].size, cTextures[i].tim2Data, textures[i].size * 2);
		header.tex.size += cTextures[i].size + 4;
	}
	header.tex.offset = sizeof(NBDHeader);
	header.amo.offset = AlignTo16(header.tex.offset + header.tex.size);
	header.amo.size = amo.getSize();
	header.ahi.offset = AlignTo16(header.amo.offset + header.amo.size);
	header.ahi.size = ahi.getSize();
	
	stream.write((char*)&header, sizeof(NBDHeader));
	for (int i = 0; i < cTextures.size(); i++)
	{
		stream.write((char*)&cTextures[i].size, sizeof(uint32_t));
		stream.write((char*)cTextures[i].tim2Data, cTextures[i].size);
	}
	for (int i = 0; i < cTextures.size(); i++)
		free(cTextures[i].tim2Data);
	amo.write(stream);
	ahi.write(stream);
}

int OBNbd::read(std::istream& stream)
{
	size_t pos = stream.tellg();
	NBDHeader header = {0};
	stream.read((char*)&header, sizeof(NBDHeader));
	
	bool correct = true;
	if (header.tex.type != 0x584554)
	{
		if (header.tex.type == 0x444954)
		{
			fprintf(OB_ERROR_OUTPUT, "[OBNbd] TID found. Room model reading/writing is not implemented yet.\n");
		}
		correct = false;
	}
	if (header.amo.type != 0x4f4d41 || header.ahi.type != 0x494841)
		correct = false;
	
	if (!correct)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBNbd] Incorrect order or unknown types encountered (%x %x %x)\n",
			header.tex.type, header.amo.type, header.ahi.type);
		return 0;
	}
	
	textures.resize(header.tex.texCount);
	char* texData = (char*) malloc(header.tex.size);
	
	stream.seekg(pos + header.tex.offset, std::ios_base::beg);
	stream.read(texData, header.tex.size);
	
	char* currentPos = texData;
	for (int i = 0; i < textures.size(); i++)
	{
		uint32_t texSize = *((uint32_t*) currentPos);
		currentPos += 4;
		uint32_t realSize = OBSld::decompress(currentPos, texSize, NULL, 0);
		textures[i].tim2Data = malloc(realSize);
		textures[i].size = realSize;
		OBSld::decompress(currentPos, texSize, textures[i].tim2Data, realSize);
		currentPos += texSize;
	}
	free(texData);
	
	stream.seekg(pos + header.amo.offset, std::ios_base::beg);
	if (!amo.read(stream))
	{
		fprintf(OB_ERROR_OUTPUT, "[OBNbd] Couldn't load AMO.\n");
		return 0;
	};
	
	stream.seekg(pos + header.ahi.offset, std::ios_base::beg);
	if (!ahi.read(stream))
	{
		fprintf(OB_ERROR_OUTPUT, "[OBNbd] Couldn't load AHI.\n");
		return 0;
	}
	
	return 1;
}

uint32_t OBNbd::getSize()
{
	return 0;
}

uint32_t OBNbd::getType()
{
	return OBType::NBD;
}