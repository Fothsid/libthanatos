#include <cstdint>
#include <thanatos/OBHierarchy.h>

OBHierarchy::OBHierarchy()
{
}

OBHierarchy::~OBHierarchy()
{
}

void OBHierarchy::write(std::ostream& stream)
{
	if (roots.size() > 0 && nodeList.size() > 0)
	{
		OBChkHeader header = { OBType::Hierarchy, 
							   1 + nodeList.size(),
							   getSize() };
		OBChkHeader rListH = { OBType::RootList & 0x7fffffff, 
							   roots.size(),
							   roots.size() * sizeof(int32_t) + 12};
		stream.write((char*)&header, sizeof(OBChkHeader));
		stream.write((char*)&rListH, sizeof(OBChkHeader));
		for (int i = 0; i < roots.size(); i++)
			stream.write((char*)&roots[i]->localId, sizeof(int32_t));
		for (int i = 0; i < nodeList.size(); i++)
			nodeList[i].write(stream);
	}
}

int OBHierarchy::read(std::istream& stream)
{
	OBChkHeader header, rootListHeader;
	stream.read((char*)&header, sizeof(OBChkHeader));

	if (header.type != OBType::Hierarchy)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBHierarchy] Unexpected chunk type encountered. (%X)\n", header.type);
		return 0;
	}

	stream.read((char*)&rootListHeader, sizeof(OBChkHeader));
	if ((rootListHeader.type | OBType::RootList) != OBType::RootList)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBHierarchy] First chunk is not a root list. (%X)\n", rootListHeader.type);
		return 0;
	}

	roots.resize(rootListHeader.count);
	nodeList.resize(header.count-1);
	for (int i = 0; i < roots.size(); i++)
	{
		int32_t rootId;
		stream.read((char*)&rootId, sizeof(int32_t));
		roots[i] = &nodeList[rootId];
	}

	for (int i = 0; i < nodeList.size(); i++)
	{
		nodeList[i].setNodeList(&nodeList[0]);
		if (!nodeList[i].read(stream))
		{
			fprintf(OB_ERROR_OUTPUT, "[OBHierarchy] Failed to read a node. (%X)\n", header.type);
			return 0;
		}
	}

	return 1;
}

uint32_t OBHierarchy::getSize()
{
	return 24 + roots.size() * sizeof(int32_t) + (sizeof(_OBAHINode) + 12) * nodeList.size();
}

uint32_t OBHierarchy::getType()
{
	return OBType::Hierarchy;
}