cmake_minimum_required(VERSION 3.20)
project(libthanatos)

include_directories(include)

set(SRC
	src/OBColorList.cpp
	src/OBJointRefList.cpp
	src/OBMaterial.cpp
	src/OBMaterialList.cpp
	src/OBMaterialRefList.cpp
	src/OBMesh.cpp
	src/OBNormalList.cpp
	src/OBObject.cpp
	src/OBPrimLists.cpp
	src/OBRenderAttribs.cpp
	src/OBTexCoordList.cpp
	src/OBTextureList.cpp
	src/OBTStripList.cpp
	src/OBTStripMaterialList.cpp
	src/OBDate.cpp
	src/OBVertexList.cpp
	src/OBWeightList.cpp
	src/OBMeshList.cpp
	src/OBModel.cpp
	src/OBNode.cpp
	src/OBHierarchy.cpp
	src/OBNbd.cpp
	)

add_library(thanatos ${SRC})

target_compile_options(thanatos PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/wd4838>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>: -Wno-sign-conversion -Wno-narrowing>
)