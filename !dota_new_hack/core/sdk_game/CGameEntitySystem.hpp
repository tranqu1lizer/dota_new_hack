#pragma once

#include "CEntityIdentity.hpp"
#include "EntityIndex.hpp"

#undef DELETE

enum class EntityPVS : uint8_t {
	UPDATE,
	LEAVE,
	CREATE,
	DELETE,
};

class C_DOTAPlayerController;

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
	//xd
	template<typename T>
	T& Field( int offset ) {
		return *(T*)( (uintptr_t)this + offset );
	}

	void* m_vtable;
public:
	CEntityIdentities* m_entity_list[MAX_ENTITY_LISTS];
	char pad_0018[ 0x108 ]; // 0x18
	CEntityIdentity* m_shit_entites;

	CEntityIdentity* find_identity( const int index )
	{
		if ( index <= -1 || index >= ( MAX_TOTAL_ENTITIES - 1 ) )
			return nullptr;

		CEntityIdentities* chunk_to_use = m_entity_list[( index / MAX_ENTITIES_IN_LIST )];
		if ( !chunk_to_use )
			return nullptr;

		CEntityIdentity* identity = &chunk_to_use->m_identities[index % MAX_ENTITIES_IN_LIST];

		if ( !identity )
			return nullptr;

		return identity;
	}
	
	template<typename T = C_BaseEntity>
	T* find_entity( int index )
	{
		if ( auto identity = find_identity( index ); identity )
			return (T*)identity->entity;
		return nullptr;
	}
	
	template<typename T = C_BaseEntity>
	T* find_entity( EntityIndex_t index )
	{
		if ( !index.is_valid( ) ) return nullptr;
		return (T*)this->find_entity( index.Get( ) );
	}

	template<typename T = C_BaseEntity>
	T* find_entity_by_handle( CHandle handle )
	{
		if ( !handle.is_valid( ) ) return nullptr;
		return (T*)this->find_entity( handle.to_index() );
	}

	FIELD( CUtlVector<IEntityListener*>, entity_listeners, 0x1548 );

	int highest_entityindex( )
	{
		return *(int*)( (uintptr_t)this + 0x1510 );
	}
};

class C_DOTA_MapTree;

class CDOTA_BinaryObjectSystem {
public:
	char pad[0x18];
	CUtlVector<C_DOTA_MapTree*> m_trees;
};