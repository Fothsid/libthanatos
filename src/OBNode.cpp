#include <cstdint>
#include <thanatos/OBNode.h>

OBNode::OBNode()
{
}

OBNode::~OBNode()
{
}

void OBNode::write(std::ostream& stream)
{
	OBChkHeader header = { type, 
						   1,
						   getSize() };
	_OBAHINode data = {0};
	data.id = localId;
	if (parent)
		data.parentId = parent->localId;
	else
		data.parentId = -1;

	if (child)
		data.childId = child->localId;
	else
		data.childId = -1;

	if (next)
		data.nextId = next->localId;
	else
		data.nextId = -1;

	data.transform = transform;
	data.groupId = groupId;
	if (type == OBType::NodeMesh)
		data.meshId = meshId;
	else
		data.meshId = -1;

	if (type == OBType::NodeJoint)
	{
		data.transform.rotation[0] = -transform.rotation[0];
		data.transform.rotation[1] = -transform.rotation[1];
		data.transform.rotation[2] = -transform.rotation[2];
		data.transform.scale[0] = 1.0f / transform.scale[0];
		data.transform.scale[1] = 1.0f / transform.scale[1];
		data.transform.scale[2] = 1.0f / transform.scale[2];
	}

	stream.write((char*)&header, sizeof(OBChkHeader));
	stream.write((char*)&data, sizeof(_OBAHINode));
}

int OBNode::read(std::istream& stream)
{
	OBChkHeader header;
	stream.read((char*)&header, sizeof(OBChkHeader));

	if (header.type != OBType::Node &&
		header.type != OBType::NodeMesh &&
		header.type != OBType::NodeJoint)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBNode] Unexpected chunk type encountered. (%X)\n", header.type);
		return 0;	
	}

	if (header.size != getSize() || header.count != 1)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBNode] Incorrect size.\n");
		return 0;
	}

	_OBAHINode data;
	stream.read((char*)&data, sizeof(_OBAHINode));
	
	type = header.type;
	transform = data.transform;

	if (type == OBType::NodeJoint)
	{
		transform.rotation[0] = -transform.rotation[0];
		transform.rotation[1] = -transform.rotation[1];
		transform.rotation[2] = -transform.rotation[2];
		transform.scale[0] = 1.0f / transform.scale[0];
		transform.scale[1] = 1.0f / transform.scale[1];
		transform.scale[2] = 1.0f / transform.scale[2];
	}

	localId = data.id;
	if (data.parentId >= 0)
		parent = &nodeList[data.parentId];
	else
		parent = NULL;

	if (data.childId >= 0)
		child = &nodeList[data.childId];
	else
		child = NULL;

	if (data.nextId >= 0)
		next = &nodeList[data.nextId];
	else
		next = NULL;

	if (type == OBType::NodeMesh || data.meshId > -1)
		meshId = data.meshId;
	groupId = data.groupId;
	return 1;
}

uint32_t OBNode::getSize()
{
	return sizeof(_OBAHINode) + 12;
}

uint32_t OBNode::getType()
{
	return type;
}

void OBNode::setNodeList(OBNode* nList)
{
	nodeList = nList;
}