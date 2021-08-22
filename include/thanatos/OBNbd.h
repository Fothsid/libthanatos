#pragma once
#include "OBObject.h"
#include "OBModel.h"
#include "OBHierarchy.h"

union OBNbdTexture
{
	struct
	{
		uint32_t size;
		void* tim2Data;
	} data;
	struct
	{
		int32_t id;
	} afs;
};

struct OBNbdScene
{
	OBModel* model = 0;
	OBHierarchy* hierarchy = 0;

	inline bool isPresent()
	{
		if (model && hierarchy)
			return true;
		else
			return false;
	}
};

struct OBShadow
{
	uint32_t size;
	void* sdwData = 0;
};

class OBNbd : public OBObject
{
public:
	OBNbd();
	virtual ~OBNbd();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	uint32_t type;
	uint32_t textureType;
	inline uint32_t getTextureStorageType() { return textureType; };
	
	std::vector<OBNbdTexture> textures;
	OBShadow shadow;

	union
	{
		struct
		{
			OBNbdScene model, addon;
		};
		struct
		{
			OBNbdScene stage, effect;
		};
	};
};