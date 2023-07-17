#pragma once

class CBaseSceneObjectDesc;
class ISceneLayer;
class CSceneObject;
using CStrongHandle = void*;

enum class RenderPrimitiveType_t : uint32_t
{
	RENDER_PRIM_POINTS = 0x0,
	RENDER_PRIM_LINES = 0x1,
	RENDER_PRIM_LINES_WITH_ADJACENCY = 0x2,
	RENDER_PRIM_LINE_STRIP = 0x3,
	RENDER_PRIM_LINE_STRIP_WITH_ADJACENCY = 0x4,
	RENDER_PRIM_TRIANGLES = 0x5,
	RENDER_PRIM_TRIANGLES_WITH_ADJACENCY = 0x6,
	RENDER_PRIM_TRIANGLE_STRIP = 0x7,
	RENDER_PRIM_TRIANGLE_STRIP_WITH_ADJACENCY = 0x8,
	RENDER_PRIM_INSTANCED_QUADS = 0x9,
	RENDER_PRIM_HETEROGENOUS = 0xa,
	RENDER_PRIM_1_CONTROL_POINT_PATCHLIST = 0xb,
	RENDER_PRIM_2_CONTROL_POINT_PATCHLIST = 0xc,
	RENDER_PRIM_3_CONTROL_POINT_PATCHLIST = 0xd,
	RENDER_PRIM_4_CONTROL_POINT_PATCHLIST = 0xe,
	RENDER_PRIM_5_CONTROL_POINT_PATCHLIST = 0xf,
	RENDER_PRIM_6_CONTROL_POINT_PATCHLIST = 0x10,
	RENDER_PRIM_7_CONTROL_POINT_PATCHLIST = 0x11,
	RENDER_PRIM_8_CONTROL_POINT_PATCHLIST = 0x12,
	RENDER_PRIM_9_CONTROL_POINT_PATCHLIST = 0x13,
	RENDER_PRIM_10_CONTROL_POINT_PATCHLIST = 0x14,
	RENDER_PRIM_11_CONTROL_POINT_PATCHLIST = 0x15,
	RENDER_PRIM_12_CONTROL_POINT_PATCHLIST = 0x16,
	RENDER_PRIM_13_CONTROL_POINT_PATCHLIST = 0x17,
	RENDER_PRIM_14_CONTROL_POINT_PATCHLIST = 0x18,
	RENDER_PRIM_15_CONTROL_POINT_PATCHLIST = 0x19,
	RENDER_PRIM_16_CONTROL_POINT_PATCHLIST = 0x1a,
	RENDER_PRIM_17_CONTROL_POINT_PATCHLIST = 0x1b,
	RENDER_PRIM_18_CONTROL_POINT_PATCHLIST = 0x1c,
	RENDER_PRIM_19_CONTROL_POINT_PATCHLIST = 0x1d,
	RENDER_PRIM_20_CONTROL_POINT_PATCHLIST = 0x1e,
	RENDER_PRIM_21_CONTROL_POINT_PATCHLIST = 0x1f,
	RENDER_PRIM_22_CONTROL_POINT_PATCHLIST = 0x20,
	RENDER_PRIM_23_CONTROL_POINT_PATCHLIST = 0x21,
	RENDER_PRIM_24_CONTROL_POINT_PATCHLIST = 0x22,
	RENDER_PRIM_25_CONTROL_POINT_PATCHLIST = 0x23,
	RENDER_PRIM_26_CONTROL_POINT_PATCHLIST = 0x24,
	RENDER_PRIM_27_CONTROL_POINT_PATCHLIST = 0x25,
	RENDER_PRIM_28_CONTROL_POINT_PATCHLIST = 0x26,
	RENDER_PRIM_29_CONTROL_POINT_PATCHLIST = 0x27,
	RENDER_PRIM_30_CONTROL_POINT_PATCHLIST = 0x28,
	RENDER_PRIM_31_CONTROL_POINT_PATCHLIST = 0x29,
	RENDER_PRIM_32_CONTROL_POINT_PATCHLIST = 0x2a,
	RENDER_PRIM_COMPUTE_SHADER = 0x2b,
	RENDER_PRIM_TYPE_COUNT = 0x2c,
};

class CRenderBufferBinding
{
public:
	uint64_t m_hBuffer; // 0x0	
	uint32_t m_nBindOffsetBytes; // 0x10	
};

class CMaterialDrawDescriptor
{
public:
	RenderPrimitiveType_t m_nPrimitiveType; // 0x0	
	int32_t m_nBaseVertex; // 0x4	
	int32_t m_nVertexCount; // 0x8	
	int32_t m_nStartIndex; // 0xc	
	int32_t m_nIndexCount; // 0x10	
	float m_flUvDensity; // 0x14	
	vector3d m_vTintColor; // 0x18	
	float m_flAlpha; // 0x24	
private:
	[[maybe_unused]] uint8_t __pad0028[ 0x4 ]; // 0x28
public:
	uint32_t m_nFirstMeshlet; // 0x2c	
	uint16_t m_nNumMeshlets; // 0x30	
private:
	[[maybe_unused]] uint8_t __pad0032[ 0x86 ]; // 0x32
public:
	CRenderBufferBinding m_indexBuffer; // 0xb8	
private:
	[[maybe_unused]] uint8_t __pad00d0[ 0x10 ]; // 0xd0
public:
	CStrongHandle m_material;
};

class CWorldSceneObjectRef {
	char pad[ 0x18 ];
public:
	CSceneObject* m_scene_object; // 0x18
};

class CSceneWorld : VClass {
public:
	int GetSceneObjectCount( ) {
		if ( IsValidCodePtr( util::vmt( (uintptr_t)this, 11 ) ) )
			return CallVFunc<11, int>( );

		return -1;
	}

	auto GetAllSceneObjects( ) {
		CUtlVector<void*> objects;

		CallVFunc<10, void>( &objects );
		return objects;
	}
};

class ISceneSystem : VClass {
public:
	static auto get( )
	{
		static auto inst = static_cast<ISceneSystem*>( util::get_interface( "scenesystem.dll", "SceneSystem_002" ) );

		return inst;
	}

	auto GetAllActiveSceneWorld( ) {
		CUtlVector<CSceneWorld*> scenes;

		CallVFunc<90, void>( &scenes );
		return scenes.stl_vector( );
	}
};