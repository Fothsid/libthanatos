#pragma once
#include "OBObject.h"
#include "OBPrimLists.h"
#include "OBMaterialRefList.h"
#include "OBTStripMaterialList.h"
#include "OBVertexList.h"
#include "OBNormalList.h"
#include "OBTexCoordList.h"
#include "OBColorList.h"
#include "OBWeightList.h"
#include "OBJointRefList.h"
#include "OBRenderAttribs.h"

class OBMesh : public OBObject
{
public:
	OBMesh();
	virtual ~OBMesh();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	int getSubchunkCount();

	OBPrimLists*          primLists          = 0;
	OBMaterialRefList*    materialRefList    = 0;
	OBTStripMaterialList* tStripMaterialList = 0;
	OBVertexList*         vertexList         = 0;
	OBNormalList*         normalList         = 0;
	OBTexCoordList*       texCoordList       = 0;
	OBColorList*          colorList          = 0;
	OBWeightList*         weightList         = 0;
	OBJointRefList*       jointRefList       = 0;
	OBRenderAttribs*      renderAttribs      = 0;
};