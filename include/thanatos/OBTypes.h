#pragma once
#include <climits>

#ifndef OB_ERROR_OUTPUT
#define OB_ERROR_OUTPUT (stderr)
#endif

namespace OBType
{
	enum
	{
		/*           CUSTOM            */
		NBD                = 0x7ffffff0,
		RoomNBD            = 0x7ffffff1,

		/*            AMO            */
		TextureRef         = 0x00000000, // Done
		Model              = 0x00000001, // Done
		MeshList           = 0x00000002, // Done
		Mesh               = 0x00000004, // Done
		PrimLists          = 0x00000005, // Done
		MaterialList       = 0x00000009, // Done
		TextureList        = 0x0000000A, // Done
		Date               = 0x00020000, // Done
		TStripListFW       = 0x00030000, // Done
		TStripListPW       = 0x00040000, // Done
		MaterialRefList    = 0x00050000, // Done
		TStripMaterialList = 0x00060000, // Done
		VertexList         = 0x00070000, // Done
		NormalList         = 0x00080000, // Done
		TexCoordList       = 0x000A0000, // Done
		ColorList          = 0x000B0000, // Done
		WeightList         = 0x000C0000, // Done
		RenderAttribs      = 0x000F0000, // Done 
		JointRefList       = 0x00100000, // Done
		MaterialConstant   = 0x00120000, // Done
		MaterialLambert    = 0x00130000, // Done
		MaterialPhong      = 0x00140000, // Done
		MaterialVertColors = 0x00150000, // Done
		
		/*            AHI            */
		Hierarchy          = 0xC0000000,
		RootList           = 0x00000000,
		Node               = 0x40000001,
		NodeMesh           = 0x40000002,
		NodeJoint          = 0x40000003,
	};
};