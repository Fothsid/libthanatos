#include <cstdint>
#include <thanatos/OBMesh.h>

OBMesh::OBMesh()
{
}

OBMesh::~OBMesh()
{
	if (primLists)          delete primLists;
	if (materialRefList)    delete materialRefList;
	if (tStripMaterialList) delete tStripMaterialList;
	if (vertexList)         delete vertexList;
	if (normalList)         delete normalList;
	if (texCoordList)       delete texCoordList;
	if (colorList)          delete colorList;
	if (weightList)         delete weightList;
	if (jointRefList)       delete jointRefList;
	if (renderAttribs)      delete renderAttribs;
}

void OBMesh::write(std::ostream& stream)
{
	OBChkHeader header = { getType(),
		                   getSubchunkCount(), 
		                   getSize() };
	stream.write((char*)&header, sizeof(OBChkHeader));
	if (primLists)          primLists->write(stream);
	if (materialRefList)    materialRefList->write(stream);
	if (tStripMaterialList) tStripMaterialList->write(stream);
	if (vertexList)         vertexList->write(stream);
	if (normalList)         normalList->write(stream);
	if (texCoordList)       texCoordList->write(stream);
	if (colorList)          colorList->write(stream);
	if (weightList)         weightList->write(stream);
	if (jointRefList)       jointRefList->write(stream);
	if (renderAttribs)      renderAttribs->write(stream);
}

int OBMesh::read(std::istream& stream)
{
	OBChkHeader header;
	stream.read((char*)&header, sizeof(OBChkHeader));

	if (header.type != OBType::Mesh)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBMesh] Unexpected chunk type encountered. (%X)\n", header.type);
		return 0;
	}

	int meshStart = stream.tellg();
	for (uint32_t i = 0; i < header.count; i++)
	{
		OBChkHeader cHead;
		
		int len = stream.tellg();
		if (len - meshStart + 12 >= header.size)
		{
			fprintf(OB_ERROR_OUTPUT, "[OBMesh] Warning: Incorrect mesh subchunk count (it it's EFxx.NBD or File1 OBJSxx.NBD models then don't worry, those are broken)\n");
			break;
		}
		stream.read((char*)&cHead, sizeof(OBChkHeader));
		stream.seekg(len, std::ios_base::beg);

		OBObject* child = NULL;
		bool unexpected = false;
		switch (cHead.type)
		{
			case OBType::PrimLists:          primLists          = new OBPrimLists();          child = (OBObject*) primLists; break;
			case OBType::MaterialRefList:    materialRefList    = new OBMaterialRefList();    child = (OBObject*) materialRefList; break;
			case OBType::TStripMaterialList: tStripMaterialList = new OBTStripMaterialList(); child = (OBObject*) tStripMaterialList; break;
			case OBType::VertexList:         vertexList         = new OBVertexList();         child = (OBObject*) vertexList; break;
			case OBType::NormalList:         normalList         = new OBNormalList();         child = (OBObject*) normalList; break;
			case OBType::TexCoordList:       texCoordList       = new OBTexCoordList();       child = (OBObject*) texCoordList; break;
			case OBType::ColorList:          colorList          = new OBColorList();          child = (OBObject*) colorList; break;
			case OBType::WeightList:         weightList         = new OBWeightList();         child = (OBObject*) weightList; break;
			case OBType::JointRefList:       jointRefList       = new OBJointRefList();       child = (OBObject*) jointRefList; break;
			case OBType::RenderAttribs:      renderAttribs      = new OBRenderAttribs();      child = (OBObject*) renderAttribs; break;
			default: unexpected = true; break;
		}
		if (!child)
		{
			if (unexpected)
				fprintf(OB_ERROR_OUTPUT, "[OBMesh] Unexpected chunk: %X (pos:%X)\n", cHead.type, stream.tellg());
			else
				fprintf(OB_ERROR_OUTPUT, "[OBMesh] Could not allocate memory for chunk type %X\n", cHead.type);
			return 0;
		}
		if (!child->read(stream))
		{
			fprintf(OB_ERROR_OUTPUT, "[OBMesh] Failed to read chunk: %X\n", cHead.type);
			return 0;
		}
	}

	return 1;
}

uint32_t OBMesh::getSize()
{
	int size = 12;
	if (primLists)          size += primLists->getSize();
	if (materialRefList)    size += materialRefList->getSize();
	if (tStripMaterialList) size += tStripMaterialList->getSize();
	if (vertexList)         size += vertexList->getSize();
	if (normalList)         size += normalList->getSize();
	if (texCoordList)       size += texCoordList->getSize();
	if (colorList)          size += colorList->getSize();
	if (weightList)         size += weightList->getSize();
	if (jointRefList)       size += jointRefList->getSize();
	if (renderAttribs)      size += renderAttribs->getSize();
	return size;
}

uint32_t OBMesh::getType()
{
	return OBType::Mesh;
}

int OBMesh::getSubchunkCount()
{
	int count = 0;
	if (primLists)          count++;
	if (materialRefList)    count++;
	if (tStripMaterialList) count++;
	if (vertexList)         count++;
	if (normalList)         count++;
	if (texCoordList)       count++;
	if (colorList)          count++;
	if (weightList)         count++;
	if (jointRefList)       count++;
	if (renderAttribs)      count++;
	return count;
}