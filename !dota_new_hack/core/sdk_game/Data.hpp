#pragma once

#include "../../main.hpp"

class entity_list {
public:
	std::unordered_map<uint16_t, C_BaseEntity*> m;
	std::unordered_set<C_DOTA_BaseNPC_Hero*> heroes;

	C_BaseEntity* operator[]( const std::string_view& name ) {
		for ( auto& [idx, ent] : m ) {

			if ( !ent->identity( )->has_name( ) && ent->schema_dynamic_binding( ) )
				continue;

			if ( name == ent->identity( )->entity_name_view( ) || name == ent->schema_dynamic_binding( )->m_binary_name ) {
				return ent;
			}
		}

		return nullptr;
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
};

class CContext {
public:
	entity_list entities;

	INetChannel* m_net_channel;
	vector3d traced_cursor;
	CUIPanel* ui_hud;
	bool ui_hud_status = false;
};

inline CContext context;