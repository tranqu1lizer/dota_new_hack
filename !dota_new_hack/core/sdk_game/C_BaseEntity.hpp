#pragma once

#include "../../main.hpp"
#include "CDOTA_ModifierManager.hpp"
#include "CHandle.hpp"
#include "../functions/functions.hpp"
#include "C_CollisionProperty.hpp"
#include "CEntityIdentity.hpp"
#include "CGameEntitySystem.hpp"

#include "NormalClass.hpp"

enum DOTATeam_t : int {
	DOTA_TEAM_INVALID = -1,
	DOTA_TEAM_FIRST = 2,
	DOTA_TEAM_GOODGUYS = 2, //Radiant team.
	DOTA_TEAM_BADGUYS = 3, 	//Dire team.
	DOTA_TEAM_NEUTRALS = 4, //Neutral.
	DOTA_TEAM_NOTEAM = 5,
	DOTA_TEAM_CUSTOM_1 = 6,
	DOTA_TEAM_CUSTOM_MIN = 6,
	DOTA_TEAM_CUSTOM_2 = 7,
	DOTA_TEAM_CUSTOM_3 = 8,
	DOTA_TEAM_CUSTOM_COUNT = 8,
	DOTA_TEAM_CUSTOM_4 = 9,
	DOTA_TEAM_CUSTOM_5 = 10,
	DOTA_TEAM_CUSTOM_6 = 11,
	DOTA_TEAM_CUSTOM_7 = 12,
	DOTA_TEAM_CUSTOM_8 = 13,
	DOTA_TEAM_CUSTOM_MAX = 13,
	DOTA_TEAM_COUNT = 14
};

class C_BaseEntity : public VClass {
public:
	int index( ) {
		if ( !util::does_exists( this ) ) return -1;
		return this->identity( )->entHandle & 0x7FFF;
	}

	CHandle owner( ) {
		if ( !util::does_exists( this ) ) return CHandle{};
		static const auto offset = schema::dynamic_field( "client.dll/C_BaseEntity/m_hOwnerEntity" );
		return Member<CHandle>( offset );
	}

	DOTATeam_t team_number( ) {
		if ( !util::does_exists( this ) ) return DOTA_TEAM_INVALID;
		static const auto offset = schema::dynamic_field( "client.dll/C_BaseEntity/m_iTeamNum" );
		return Member<DOTATeam_t>( offset );
	}

	vector3d abs_origin( )
	{
		if ( !util::does_exists( this ) ) return vector3d{ -1,-1,-1 };
		return Member<VClass*>( schema::dynamic_field( "client.dll/C_BaseEntity/m_pGameSceneNode" ) )
			->Member<vector3d>( schema::dynamic_field( "client.dll/CGameSceneNode/m_vecAbsOrigin" ) );
	}

	void set_abs_origin( vector3d abs_origin )
	{
		if ( !util::does_exists( this ) ) return;
		static const auto scene_offset = schema::dynamic_field( "client.dll/C_BaseEntity/m_pGameSceneNode" );
		static const auto offset = schema::dynamic_field( "client.dll/CGameSceneNode/m_vecAbsOrigin" );

		Member<VClass*>( scene_offset )->Member<vector3d>( offset ) = abs_origin;
	}

	int max_health( ) {
		if ( !util::does_exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field( "client.dll/C_BaseEntity/m_iMaxHealth" );
		return Member<int>( offset );
	}

	int health( ) {
		if ( !util::does_exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field( "client.dll/C_BaseEntity/m_iHealth" );
		return Member<int>(offset);
	}

	////////////////////

	bool ally( ) {
		if ( !util::does_exists( this ) ) return false;
		return reinterpret_cast<C_BaseEntity*>( global::g_LocalEntity )->team_number( ) == this->team_number( );
	}

	__declspec( restrict ) __declspec( noalias ) const CEntityIdentity* identity( ) noexcept {
		if ( !util::does_exists( this ) ) return nullptr;
		return Member<CEntityIdentity*>( 0x10 );
	}

	__declspec( noalias ) const CSchemaClassBinding* schema_dynamic_binding( ) noexcept {
		if ( !util::does_exists( this ) ) return nullptr;
		const auto aDynamicBindingEntity = util::vmt( reinterpret_cast<uintptr_t>( this ), 0 );
		if ( !aDynamicBindingEntity ) return nullptr;

		return reinterpret_cast<CSchemaClassBinding * ( __fastcall* )( )>( aDynamicBindingEntity )( );
	}

	__declspec( noalias ) const C_CollisionProperty* collideable( ) noexcept {
		if ( !util::does_exists( this ) ) return nullptr;
		const auto aCollideable = util::vmt( reinterpret_cast<uintptr_t>( this ), 50 );
		if ( !aCollideable ) return nullptr;

		return reinterpret_cast<C_CollisionProperty * ( __fastcall* )( )>( aCollideable )( );
	}
};