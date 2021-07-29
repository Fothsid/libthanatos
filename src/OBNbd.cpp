#include <cstdint>
#include <list>
#include <algorithm>
#include <thanatos/OBNbd.h>

uint32_t SLD_Decompress(void* src, uint32_t srcSize, void* dest, uint32_t destSize)
{
	uint16_t* s = (uint16_t*) src;
	uint16_t* d = (uint16_t*) dest;
	uint32_t sPos = 0;
	uint32_t sSize = srcSize / 2;
	uint32_t dPos = 0;
	uint32_t dSize = destSize / 2;
	uint32_t bLeft = 0;
	uint16_t flag = 0;
	while (sPos < sSize && s)
	{
		if (bLeft == 0)
		{
			flag = s[sPos];
			sPos++;
			bLeft = 16;
		}
		
		if (flag & 0x8000)
		{
			uint16_t mode = s[sPos];
			sPos++;
			
			uint16_t count = mode >> 0xb;
			if (count == 0)
			{
				count = s[sPos];
				sPos++;
			}
			
			uint16_t offset = mode & 0x7ff;
			if (offset == 0)
			{
				for (int i = 0; i < count; i++)
				{
					if (d)
						d[dPos] = 0;
					dPos++;
				}
			}
			else
			{
				uint32_t pos = dPos - offset;
				for (int i = 0; i < count; i++)
				{
					if (d)
						d[dPos] = d[pos + i];
					dPos++;
				}
			}
		}
		else
		{
			uint16_t val = s[sPos];
			sPos++;
			if (d)
				d[dPos] = val;
			dPos++;
		}
		flag = flag << 1;
		bLeft--;
	}
	return dPos * 2;
}

#define CMP_PUSH_REF_TOKEN(offset, count) *d = ((offset) & 0x7FF | (((count) & 0x1F) << 0xb)); d++;
#define CMP_PUSH_VALUE(value) *d = (value); d++;

uint32_t SLD_Compress(void* src, uint32_t srcSize, void* dst, uint32_t destSize)
{
	uint16_t* s = (uint16_t*)src;
	uint16_t* d = (uint16_t*)dst;
	uint16_t* sEnd = s + (srcSize / 2);
	uint16_t* dEnd = d + (destSize / 2);
	
	uint16_t* sc = s;
	int tokenId = 0;
	uint16_t* flag = d;
	*flag = 0;
	d += 1;
	while (sc < sEnd && d < dEnd)
	{
		if (*sc == 0)
		{
			int count = 0;
			while (*(sc + count + 1) != 0 || count < 31)
				count++;
			CMP_PUSH_REF_TOKEN(0, count);
			sc += count;
			*flag = (*flag) | (1 << (15 - tokenId));
			tokenId++;
		}
		else
		{
			int offset = (int)((sc - s) < 2047 ? (sc - s) : 2047);
			while (*sc != *(sc - offset) && offset > 0)
				offset--;
			if (offset > 0)
			{
				int count = 0;
				while (*(sc + count) == *(sc - offset + count) && count < 31)
					count++;
				CMP_PUSH_REF_TOKEN(offset, count);
				sc += count;
				*flag = (*flag) | (1 << (15 - tokenId));
				tokenId++;
			}
			else
			{
				CMP_PUSH_VALUE(*sc);
				sc += 1;
				tokenId++;
			}
		}
		
		if (tokenId > 15)
		{
			tokenId = 0;
			flag = d;
			*flag = 0;
			d += 1;
		}
	}
	
	while (tokenId < 15)
	{
		CMP_PUSH_VALUE(0);
		tokenId++;
	}
	return (uint32_t)((d - ((uint16_t*)dst)) * 2);
}

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
		cTextures[i].size = SLD_Compress(textures[i].tim2Data, textures[i].size, cTextures[i].tim2Data, textures[i].size * 2);
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
			fprintf(OB_ERROR_OUTPUT, "[OBNbd] TID found. Use OBRoomNbd for room models instead.\n");
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
		uint32_t realSize = SLD_Decompress(currentPos, texSize, NULL, 0);
		textures[i].tim2Data = malloc(realSize);
		textures[i].size = realSize;
		SLD_Decompress(currentPos, texSize, textures[i].tim2Data, realSize);
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