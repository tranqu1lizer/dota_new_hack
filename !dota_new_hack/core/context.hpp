#pragma once

#include "../main.hpp"

class entity_list {
public:
	std::unordered_map<uint16_t, C_BaseEntity*> m;
	std::unordered_set<C_DOTA_BaseNPC_Hero*> heroes;

	std::vector< C_BaseEntity*> operator[]( const std::string_view& name ) {
		std::vector< C_BaseEntity*> ret;

		for ( auto& [idx, ent] : m ) {

			if ( !ent || !ent->GetIdentity( )->HasName( ) || !ent->Schema_DynamicBinding( ) )
				continue;

			if ( name == ent->GetIdentity( )->GetEntityNameView( ) || name == ent->Schema_DynamicBinding( )->m_binary_name )
			{
				ret.push_back( ent );
			}
		}

		return ret;
	}

	C_BaseEntity* operator[]( const std::uint16_t index ) {
		for ( auto& [idx, ent] : m ) {
			if ( idx == index )
				return ent;
		}

		return nullptr;
	}

	C_BaseEntity* operator[]( const EntityIndex_t index ) {
		if ( !index.is_valid( ) ) return nullptr;

		for ( auto& [idx, ent] : m ) {
			if ( idx == index.Get( ) )
				return ent;
		}

		return nullptr;
	}

	C_DOTAPlayerController* GetLocalPlayer( ) { // Wolf49406 method

		for ( C_BaseEntity* ent_player : this->operator[]( "dota_player_controller" ) ) {
			C_DOTAPlayerController* player = (C_DOTAPlayerController*)ent_player;

			if ( !player )
				continue;

			if ( player->IsLocalPlayer( ) )
			{
				return player;
			}
		}

		return nullptr;
	}
};

class CContext {
public:
	entity_list entities;

	C_DOTAPlayerController* local_controller;
	C_BaseEntity* local_entity;

	INetChannel* m_net_channel;
	vector3d traced_cursor;
	CUIPanel* DotaHud;
};

inline CContext context;