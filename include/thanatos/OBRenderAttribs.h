#pragma once
#include "OBObject.h"

struct _OBRenderAttribs
{
	int32_t version; // Always 0x10000
	int32_t material;
	int32_t specular; 
	int32_t cull;
	int32_t scissor; // Always seems to be 1
	int32_t light; // Used; but have to figure out what it does exactly
	int32_t wrap;
	int32_t uvScroll;
	int32_t disableFog;
	int32_t fadeCol;
	int32_t specialShader; // Not used.
	int32_t alphaSrc;
	int32_t alphaDst;
	int32_t alphaOp;
	int32_t zOp; // IGNORE. The game forces everything to ZOP_LESS
	int32_t zWrite;
	int32_t filtering;
	int32_t addressMode;
};

class OBRenderAttribs : public OBObject
{
public:
	OBRenderAttribs();
	virtual ~OBRenderAttribs();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	_OBRenderAttribs attribs;
};