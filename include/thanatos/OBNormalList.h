#pragma once
#include "OBObject.h"
#include "OBVertexList.h"

typedef OBVertex OBNormal;
class OBNormalList : public OBObject
{
public:
	OBNormalList();
	virtual ~OBNormalList();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	std::vector<OBNormal> list;
};