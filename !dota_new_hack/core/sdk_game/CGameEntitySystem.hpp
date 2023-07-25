#pragma once

#include "CEntityIdentity.hpp"
#include "EntityIndex.hpp"

class C_DOTAPlayerController;
class CEntity2SaveRestore;
class CEntity2NetworkClasses;

class IEntityListener {
public:
	virtual void OnEntityCreated( C_BaseEntity* ) = 0;
	virtual void OnEntitySpawned( C_BaseEntity* ) = 0;
	virtual void OnEntityDeleted( C_BaseEntity* ) = 0;
	virtual void OnEntityParentChanged( C_BaseEntity*, C_BaseEntity* parent ) = 0;
};

class CEntitySystem {
public:
	virtual void n_0( ) = 0;
	virtual void BuildResourceManifest( void ) = 0; // 01
	virtual void n_2( ) = 0;
	virtual void n_3( ) = 0;
	virtual void n_4( ) = 0;
	virtual void n_5( ) = 0;
	virtual void n_6( ) = 0;
	virtual void AddRefKeyValues( void const* ) = 0; // 7
	virtual void ReleaseKeyValues( void const* ) = 0; // 8
	virtual void n_9( ) = 0;
	virtual void n_10( ) = 0;
	virtual void ClearEntityDatabase( void ) = 0; // 11
	virtual C_BaseEntity* FindEntityProcedural( const char *... ) = 0;
	virtual C_BaseEntity* OnEntityParentChanged( C_BaseEntity* old_maybe, C_BaseEntity* new_maybe ) = 0; //13
	virtual C_BaseEntity* OnAddEntity( C_BaseEntity*, CHandle ) = 0; // 14
	virtual C_BaseEntity* OnRemoveEntity( C_BaseEntity*, CHandle ) = 0; // 15
	virtual void n_16( ) = 0;
	virtual void SortEntities( int, void*, int*, int* ) = 0; // 17
	virtual void n_18( ) = 0;
	virtual void n_19( ) = 0;
	virtual void n_20( ) = 0;
	virtual void n_21( ) = 0;
};

class CGameEntitySystem : CEntitySystem
{
	template<typename T>
	T& Field( int offset ) {
		return *(T*)( (uintptr_t)this + offset );
	}

	char pad_00[ 16 ]; // 0x0
public:
	CEntityIdentities* m_pEntityLists[ MAX_ENTITY_LISTS ]; // 0x10
private:
	char pad_210[ 4854 ]; // 0x210
public:
	std::int32_t m_highest_entindex; // 0x1510
private:
	char pad_1514[ 0x30 ];
public:
	CUtlVector<IEntityListener*> m_vecEntityEvents; // 0x1548
	CEntity2SaveRestore* m_pEnt2SaveRestore; // 0x1560
	CEntity2NetworkClasses* m_pEnt2NetClasses; // 0x1568
};

class C_DOTA_MapTree;

class CDOTA_BinaryObjectSystem {
public:
	char pad[0x18];
	CUtlVector<C_DOTA_MapTree*> m_trees;
};