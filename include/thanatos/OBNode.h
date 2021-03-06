#pragma once
#include <thanatos/OBObject.h>

struct NodeTransform
{
	float scale[4]       = {1.0f, 1.0f, 1.0f, 1.0f};
	float rotation[4]    = {0.0f, 0.0f, 0.0f, 1.0f};
	float translation[4] = {0.0f, 0.0f, 0.0f, 1.0f};
};

struct _OBAHINode
{
	int32_t id;
	int32_t parentId;
	int32_t childId;
	int32_t nextId;
	NodeTransform transform;
	int32_t meshId;
	int32_t groupId;
	int32_t _dummy0[46];
};

class OBNode : public OBObject
{
public:
	OBNode();
	virtual ~OBNode();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	void setNodeList(OBNode* nList);

	OBNode* nodeList;

	uint32_t type;
	NodeTransform transform;
	int localId;
	int meshId;
	int groupId;
	OBNode* parent = 0;
	OBNode* child = 0;
	OBNode* next = 0;
};