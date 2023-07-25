#pragma once

using CSplitScreenSlot = unsigned int;

class CNetworkServerSpawnGroup;
struct WorldNodeFlags_t;

struct BuilderParams_t {
private:
	char pad[ 0x20 ];
public:
	std::int32_t m_num_nodes;
};

class CWorld {
	char pad[ 16 ]; // 0x0
public:
	CUtlVector<CWorldSceneObjectRef*>* m_object_refs; // 0x10
private:
	char pad_18[ 16 ]; // 0x18
public:
	BuilderParams_t* m_builder_params; // 0x28
private:
	char pad_[ 0x138 ];
public:
	std::uint8_t m_flags;

	virtual void Init( CSceneWorld, bool, unsigned int ) = 0; // 0x0
	virtual void CreateAndDispatchLoadRequests( vector3d& const, int ) = 0; // 0x8
	virtual void Shutdown( void ) = 0; // 0x10
	virtual unsigned int GetNumNodes( void ) = 0; // 0x18
	virtual BuilderParams_t* GetBuilderParams( ) = 0; // 0x20
	virtual bool IsAncestor( unsigned int, unsigned int ) = 0; // 0x28
	virtual void* GetNodeData( int node_num ) = 0; // 0x30
	virtual CWorld* GetNodeBounds( int node_num ) = 0; // 0x38
	virtual float GetNodeMinDistance( int node_num ) = 0; // 0x40
	virtual void SetWorldTransform( matrix3x4_t& ) = 0; // 0x48
	virtual matrix3x4_t& GetWorldTransform( ) = 0; // 0x50
	virtual void SetLoadSun( bool ) = 0; // 0x58
	virtual bool GetLoadSun( ) = 0; // 0x60
	virtual void IsFullyLoadedForPlayer( CSplitScreenSlot, int ) = 0; // 0x68
	virtual void ClearOutstandingLoadRequests( ) = 0; // 0x70
	virtual void PrecacheAllWorldNodes( WorldNodeFlags_t, bool ) = 0; // 0x78
	virtual int* GetEntityList( char* const ) = 0; // 0x80
	virtual void FindEntitiesByTargetname( char const*, char const*, CUtlVector<void const*>& ) = 0; // 0x88
	virtual void vf18( ) = 0; // 0x90
	virtual void vf19( ) = 0; // 0x98
	virtual void vf20( ) = 0; // 0xa0
	virtual void vf21( ) = 0; // 0xa8
};

class CSingleWorldRep {
	char pad_00[ 8 ]; // 0x0
public:
	char* m_name; // 0x8
private:
	char pad_10[ 32 ]; // 0x10
public:
	CWorld* m_world; // 0x30
private:
	char pad_38[ 8 ]; // 0x38
public:
	CNetworkServerSpawnGroup* m_net_server_spawngroup; // 0x40
	CSceneWorld* m_scene_world; // 0x48
};

class IWorldRendererMgr : VClass {
	PDEFINE_INTERFACE( IWorldRendererMgr, "worldrenderer.dll", "WorldRendererMgr001" );

	PAD( 0x88 ); // 0x0
	CUtlVector<CSingleWorldRep*> m_single_worlds; // 0x90
};