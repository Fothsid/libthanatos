#pragma once
#include "OBObject.h"
#include "OBNode.h"

class OBHierarchy : public OBObject
{
public:
	OBHierarchy();
	virtual ~OBHierarchy();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	std::vector<OBNode*> roots;
	std::vector<OBNode> nodeList;
};