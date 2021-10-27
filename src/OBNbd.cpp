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
	
	inline bool isValid()
	{
		switch (type)
		{
			case 0x584554: // TEX
			case 0x444954: // TID
			case 0x4f4d41: // AMO
			case 0x494841: // AHI
			case 0x574453: // SDW
				break;
			default: return false;
		}

		if (offset == 0 || size == 0)
			return false;

		if (type == 0x584554 && texCount == 0)
			return false;

		return true;
	}
};

struct NBDPair
{
	NBDEntry amo, ahi;

	inline bool isValid()
	{
		if (amo.type != 0x4f4d41 && !amo.isValid())
			return false;
		if (ahi.type != 0x494841 && !ahi.isValid())
			return false;
		return true;
	}
};

struct NBDHeader
{
	NBDEntry tex;
	union
	{
		struct
		{
			NBDPair  model;
			NBDEntry dummy0;
			NBDEntry shadow;
			NBDPair  addon;
			NBDEntry dummy1;
		} regular;
		struct
		{
			NBDPair stage;
			NBDPair effect;
			NBDEntry dummy0;
			NBDEntry dummy1;
		} room;
	};

	inline int determineType()
	{
		if (tex.type == 0x584554 &&
			regular.model.isValid())
			return OBType::NBD;
		else if (tex.type == 0x444954 &&
				 room.stage.isValid() &&
				 room.effect.isValid())
			return OBType::RoomNBD;
		else
			return OBType::Invalid;
	}
};

OBNbd::OBNbd()
{
	memset(&model, 0, sizeof(OBNbdScene) * 2);
}

OBNbd::~OBNbd()
{
	for (int i = 0; i < textures.size(); i++)
	{
		if (textures[i].data.tim2Data)
			free(textures[i].data.tim2Data);
	}

	if (shadow.sdwData)
		free(shadow.sdwData);
}

#define AlignTo16(X) (((X) + 15) & ~15)
#define FillBytes(X) \
    for (int64_t __i = 0; __i < (X); __i++) \
    { \
		uint8_t b = 0; stream.write((char*)&b, 1);\
    }

static void AlignStream(std::ostream& stream)
{
	int64_t byteCount = (AlignTo16((int64_t)stream.tellp())) - (int64_t)stream.tellp();
	FillBytes(byteCount);
}

void OBNbd::write(std::ostream& stream)
{
	if (type != OBType::NBD && type != OBType::RoomNBD)
		return;
	if (textureType != OBType::TextureData && textureType != OBType::TextureAFSRef)
		return;

	NBDHeader header = {0};
	if (textureType == OBType::TextureData)
		header.tex.type = 0x584554;
	else if (textureType == OBType::TextureAFSRef)
		header.tex.type = 0x444954;

	if (type == OBType::NBD)
	{
		if (textureType != OBType::TextureData)
		{
			fprintf(OB_ERROR_OUTPUT, "[OBNbd] Regular NBD doesn't support this texture type.\n");
			return;
		}
		std::vector<OBNbdTexture> cTextures(textures.size());
		header.tex.texCount = (uint32_t) textures.size();
		header.tex.size = 0;
		for (int i = 0; i < cTextures.size(); i++)
		{
			if (textures[i].data.tim2Data)
			{
				cTextures[i].data.tim2Data = malloc(textures[i].data.size * 3);
				if (!cTextures[i].data.tim2Data)
				{
					fprintf(OB_ERROR_OUTPUT, "[OBNbd] Could not allocate memory for texture compression\n");
				}
				cTextures[i].data.size = OBSld::compress(textures[i].data.tim2Data, textures[i].data.size, cTextures[i].data.tim2Data, textures[i].data.size * 3);
				header.tex.size += cTextures[i].data.size + 4;
			}
		}
		header.tex.offset = sizeof(NBDHeader);

		NBDPair* pair = &header.regular.model;
		pair->amo.type = 0x4F4D41;
		pair->ahi.type = 0x494841;
		pair->amo.offset = AlignTo16(header.tex.offset + header.tex.size);
		pair->amo.size = model.model->getSize();
		pair->ahi.offset = AlignTo16(pair->amo.offset + pair->amo.size);
		pair->ahi.size = model.hierarchy->getSize();

		uint32_t nextOffset = AlignTo16(pair->ahi.offset + pair->ahi.size);
		if (shadow.sdwData)
		{
			header.regular.shadow.type = 0x574453;
			header.regular.shadow.offset = nextOffset;
			header.regular.shadow.size = shadow.size;
			nextOffset = AlignTo16(header.regular.shadow.offset + header.regular.shadow.size);
		}

		pair = &header.regular.addon;
		if (addon.isPresent())
		{
			pair->amo.type = 0x4F4D41;
			pair->ahi.type = 0x494841;
			pair->amo.offset = nextOffset;
			pair->amo.size = addon.model->getSize();
			pair->ahi.offset = AlignTo16(pair->amo.offset + pair->amo.size);
			pair->ahi.size = addon.hierarchy->getSize();
		}

		stream.write((char*)&header, sizeof(NBDHeader));
		for (int i = 0; i < cTextures.size(); i++)
		{
			if (cTextures[i].data.tim2Data)
			{
				stream.write((char*)&cTextures[i].data.size, sizeof(uint32_t));
				stream.write((char*)cTextures[i].data.tim2Data, cTextures[i].data.size);
			}
		}

		for (int i = 0; i < cTextures.size(); i++)
		{
			if (cTextures[i].data.tim2Data)
				free(cTextures[i].data.tim2Data);
		}

		pair = &header.regular.model;

		AlignStream(stream);
		model.model->write(stream);
		
		AlignStream(stream);
		model.hierarchy->write(stream);

		if (shadow.sdwData)
		{
			AlignStream(stream);
			stream.write((char*)shadow.sdwData, shadow.size);
		}
	
		pair = &header.regular.addon;
		if (addon.isPresent())
		{
			AlignStream(stream);
			addon.model->write(stream);
			
			AlignStream(stream);
			addon.hierarchy->write(stream);
		}
		
	}
	else
	{
		if (textureType != OBType::TextureAFSRef)
		{
			fprintf(OB_ERROR_OUTPUT, "[OBNbd] Room NBD doesn't support this texture type.\n");
			return;
		}

		header.tex.offset = sizeof(NBDHeader);
		header.tex.size = (textures.size()+1) * sizeof(uint32_t);

		NBDPair* pair = &header.room.stage;
		pair->amo.type = 0x4F4D41;
		pair->ahi.type = 0x494841;
		pair->amo.offset = AlignTo16(header.tex.offset + header.tex.size);
		pair->amo.size = stage.model->getSize();
		pair->ahi.offset = AlignTo16(pair->amo.offset + pair->amo.size);
		pair->ahi.size = stage.hierarchy->getSize();

		uint32_t nextOffset = AlignTo16(pair->ahi.offset + pair->ahi.size);

		pair = &header.room.effect;
		pair->amo.type = 0x4F4D41;
		pair->ahi.type = 0x494841;
		pair->amo.offset = nextOffset;
		pair->amo.size = effect.model->getSize();
		pair->ahi.offset = AlignTo16(pair->amo.offset + pair->amo.size);
		pair->ahi.size = effect.hierarchy->getSize();

		int32_t textureCount = textures.size();
		stream.write((char*)&header, sizeof(NBDHeader));
		stream.write((char*)&textureCount, sizeof(int32_t));
		for (int i = 0; i < textures.size(); i++)
		{
			stream.write((char*)&textures[i].afs.id, sizeof(int32_t));
		}

		pair = &header.room.stage;
		AlignStream(stream);
		stage.model->write(stream);
		
		AlignStream(stream);
		stage.hierarchy->write(stream);
	
		pair = &header.room.effect;
		AlignStream(stream);
		effect.model->write(stream);
		
		AlignStream(stream);
		effect.hierarchy->write(stream);
	}
}

static int ReadAMOAHIPair(size_t begin, std::istream& stream, NBDPair& pair, OBNbdScene& scene)
{
	scene.model = new OBModel();
	scene.hierarchy = new OBHierarchy();

	if (!pair.isValid())
	{
		fprintf(OB_ERROR_OUTPUT, "[OBNbd] Not a valid AMO-AHI pair.\n");
		return 0;
	}

	stream.seekg(begin + pair.amo.offset, std::ios_base::beg);
	if (!scene.model->read(stream))
	{
		fprintf(OB_ERROR_OUTPUT, "[OBNbd] Couldn't load model (AMO).\n");
		return 0;
	};
	
	stream.seekg(begin + pair.ahi.offset, std::ios_base::beg);
	if (!scene.hierarchy->read(stream))
	{
		fprintf(OB_ERROR_OUTPUT, "[OBNbd] Couldn't load hierarchy (AHI).\n");
		return 0;
	}

	return 1;
}

int OBNbd::read(std::istream& stream)
{
	size_t begin = stream.tellg();
	NBDHeader header = {0};
	stream.read((char*)&header, sizeof(NBDHeader));

	type = header.determineType();
	if (type == OBType::Invalid)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBNbd] Invalid NBD header.\n");
		return 0;
	}

	fprintf(OB_INFO_OUTPUT, "[OBNbd] Type: %s.\n", OBType::getName(type));
	if (type == OBType::NBD && header.tex.isValid())
	{
		textureType = OBType::TextureData; 
		textures.resize(header.tex.texCount);

		char* texData = (char*) malloc(header.tex.size);
		stream.seekg(begin + header.tex.offset, std::ios_base::beg);
		stream.read(texData, header.tex.size);
		
		char* currentPos = texData;
		for (int i = 0; i < textures.size(); i++)
		{
			uint32_t texSize = *((uint32_t*) currentPos);
			currentPos += 4;
			uint32_t realSize = OBSld::decompress(currentPos, texSize, NULL, 0);
			textures[i].data.tim2Data = malloc(realSize);
			textures[i].data.size = realSize;
			OBSld::decompress(currentPos, texSize, textures[i].data.tim2Data, realSize);
			currentPos += texSize;
		}
		free(texData);

		int status = ReadAMOAHIPair(begin, stream, header.regular.model, model);
		if (!status)
		{
			fprintf(OB_ERROR_OUTPUT, "[OBNbd] Couldn't load the main AMO/AHI pair.\n");
			return 0;
		}

		bool hasSdw = header.regular.shadow.isValid() && header.regular.shadow.type == 0x574453;
		fprintf(OB_INFO_OUTPUT, "[OBNbd] Shadow: %s.\n", hasSdw ? "Yes" : "No");
		if (hasSdw)
		{
			shadow.size = header.regular.shadow.size;
			shadow.sdwData = malloc(shadow.size);
			stream.seekg(begin + header.regular.shadow.offset, std::ios_base::beg);
			stream.read((char*)shadow.sdwData, shadow.size);
		}

		fprintf(OB_INFO_OUTPUT, "[OBNbd] Additional AMO/AHI pair: %s.\n", header.regular.addon.isValid() ? "Yes" : "No");
		if (header.regular.addon.isValid())
		{
			status = ReadAMOAHIPair(begin, stream, header.regular.addon, addon);
			if (!status)
			{
				fprintf(OB_ERROR_OUTPUT, "[OBNbd] Couldn't load the additional AMO/AHI pair.\n");
				delete addon.model;
				delete addon.hierarchy;
			}
		}
	}
	else if (type == OBType::RoomNBD && header.tex.isValid())
	{
		textureType = OBType::TextureAFSRef;
		int32_t textureCount = 0;
		stream.seekg(begin + header.tex.offset, std::ios_base::beg);
		stream.read((char*)&textureCount, sizeof(int32_t));
		textures.resize(textureCount);
		std::vector<int32_t> textureIds(textureCount);

		stream.read((char*)&textureIds[0], sizeof(int32_t) * textureCount);
		for (int i = 0; i < textureCount; i++)
			textures[i].afs.id = textureIds[i];

		int status = ReadAMOAHIPair(begin, stream, header.room.stage, stage);
		if (!status)
		{
			fprintf(OB_ERROR_OUTPUT, "[OBNbd] Couldn't load the stage AMO/AHI pair.\n");
			return 0;
		}

		status = ReadAMOAHIPair(begin, stream, header.room.effect, effect);
		if (!status)
		{
			fprintf(OB_ERROR_OUTPUT, "[OBNbd] Couldn't load the room AMO/AHI pair.\n");
			return 0;
		}
	}
	else
	{
		textureType = OBType::Invalid;
	}
	
	return 1;
}

uint32_t OBNbd::getSize()
{
	return 0;
}

uint32_t OBNbd::getType()
{
	return type;
}