#pragma once
#include <climits>
#include <cstdint>

#ifndef OB_ERROR_OUTPUT
#define OB_ERROR_OUTPUT (stderr)
#endif

#ifndef OB_INFO_OUTPUT
#define OB_INFO_OUTPUT (stdout)
#endif

#define OBTYPE_LIST \
	OBTYPE_DECL(Invalid,       0x7fffffff) \
	OBTYPE_DECL(NBD,           0x7fff0001) \
	OBTYPE_DECL(RoomNBD,       0x7fff0002) \
	OBTYPE_DECL(TextureData,   0x7fff0101) \
	OBTYPE_DECL(TextureAFSRef, 0x7fff0102) \
	\
	OBTYPE_DECL(TextureRef,         0x00000000) \
	OBTYPE_DECL(Model,              0x00000001) \
	OBTYPE_DECL(MeshList,           0x00000002) \
	OBTYPE_DECL(Mesh,               0x00000004) \
	OBTYPE_DECL(PrimLists,          0x00000005) \
	OBTYPE_DECL(MaterialList,       0x00000009) \
	OBTYPE_DECL(TextureList,        0x0000000A) \
	OBTYPE_DECL(Date,               0x00020000) \
	OBTYPE_DECL(TStripListFW,       0x00030000) \
	OBTYPE_DECL(TStripListPW,       0x00040000) \
	OBTYPE_DECL(MaterialRefList,    0x00050000) \
	OBTYPE_DECL(TStripMaterialList, 0x00060000) \
	OBTYPE_DECL(VertexList,         0x00070000) \
	OBTYPE_DECL(NormalList,         0x00080000) \
	OBTYPE_DECL(TexCoordList,       0x000A0000) \
	OBTYPE_DECL(ColorList,          0x000B0000) \
	OBTYPE_DECL(WeightList,         0x000C0000) \
	OBTYPE_DECL(RenderAttribs,      0x000F0000) \
	OBTYPE_DECL(JointRefList,       0x00100000) \
	OBTYPE_DECL(MaterialConstant,   0x00120000) \
	OBTYPE_DECL(MaterialLambert,    0x00130000) \
	OBTYPE_DECL(MaterialPhong,      0x00140000) \
	OBTYPE_DECL(MaterialVertColors, 0x00150000) \
	\
	OBTYPE_DECL(Hierarchy, 0xC0000000) \
	OBTYPE_DECL(RootList,  0x80000000) \
	OBTYPE_DECL(Node,      0x40000001) \
	OBTYPE_DECL(NodeMesh,  0x40000002) \
	OBTYPE_DECL(NodeJoint, 0x40000003) \

#define OBTYPE_DECL(name, value) \
	name = (value), \

namespace OBType
{
	enum
	{
		OBTYPE_LIST
	};

	const char* getName(uint32_t type);
};

#undef OBTYPE_DECL