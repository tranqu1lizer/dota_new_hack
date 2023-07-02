#pragma once

#include "CDOTA_ModifierManager.hpp"
#include "CHandle.hpp"
#include "CEntityIdentity.hpp"
#include "CGameEntitySystem.hpp"

#include "NormalClass.hpp"
#include "ISchemaSystem.hpp"
#include "CSource2Client.hpp"
#include "../util/util.hpp"

enum DataUpdateType_t {
	DATA_UPDATE_CREATED = 0,  // indicates it was created +and+ entered the pvs
	//  DATA_UPDATE_ENTERED_PVS,
	DATA_UPDATE_DATATABLE_CHANGED,
	//  DATA_UPDATE_LEFT_PVS,
	//  DATA_UPDATE_DESTROYED,    // FIXME: Could enable this, but it's a little worrying
					// since it changes a bunch of existing code
};

enum RenderMode_t {
	kRenderNormal = 0,		// src
	kRenderTransColor,		// c*a+dest*(1-a)
	kRenderTransTexture,	// src*a+dest*(1-a)
	kRenderGlow,			// src*a+dest -- No Z buffer checks -- Fixed size in screen space
	kRenderTransAlpha,		// src*srca+dest*(1-srca)
	kRenderTransAdd,		// src*a+dest
	kRenderEnvironmental,	// not drawn, used for environmental effects
	kRenderTransAddFrameBlend, // use a fractional frame value to blend between animation frames
	kRenderTransAlphaAdd,	// src + dest*(1-a)
	kRenderWorldGlow,		// Same as kRenderGlow but not fixed size in screen space
	kRenderNone,			// Don't render.

	kRenderModeCount,		// must be last
};

enum class DOTATeam_t : int {
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

class C_BaseEntity : public SchemaVClass {
public:
	CSchemaClassBinding* schema_dynamic_binding( ) {
		return CallVFunc<0, CSchemaClassBinding*>( );
	}

	ClientClass* client_class( ) {
		return CallVFunc<33, ClientClass*>( );
	}

	////////////////////

	int index( ) {
		if ( !util::exists( this ) ) return -1;
		return this->identity( )->entHandle & 0x7FFF;
	}

	CHandle owner( ) {
		if ( !util::exists( this ) ) return CHandle{};
		static const auto offset = schema::dynamic_field_offset( "client.dll/C_BaseEntity/m_hOwnerEntity" );
		return Member<CHandle>( offset );
	}

	DOTATeam_t team_number( ) {
		if ( !util::exists( this ) ) return DOTATeam_t::DOTA_TEAM_INVALID;
		static const auto offset = schema::dynamic_field_offset( "client.dll/C_BaseEntity/m_iTeamNum" );
		return Member<DOTATeam_t>( offset );
	}

	vector3d abs_origin( )
	{
		if ( !util::exists( this ) ) return vector3d{ -1,-1,-1 };

		const auto scene = *(uintptr_t*)( (uintptr_t)this + schema::dynamic_field_offset( "client.dll/C_BaseEntity/m_pGameSceneNode" ) );

		return *(vector3d*)( scene + schema::dynamic_field_offset( "client.dll/CGameSceneNode/m_vecAbsOrigin" ) );
	}

	float rotation( ) {
		return schema_member<SchemaVClass*>( "client.dll/C_BaseEntity/m_pGameSceneNode" )
			->schema_member<QAngle>( "client.dll/CGameSceneNode/m_angRotation" ).roll_deg;
	}

	// Gets the point in front of the entity at the specified distance
	vector3d forward_vector( float dist ) {
		const auto rot = rotation( );
		const float casted_rot = rotation( ) * M_PI / 180;

		const float sine = sinf( casted_rot ), cosine = cosf( casted_rot );
		return abs_origin( ) + vector3d( cosine * dist, sine * dist, 0 );
	}

	void set_abs_origin( vector3d abs_origin )
	{
		if ( !util::exists( this ) ) return;
		static const auto scene_offset = schema::dynamic_field_offset( "client.dll/C_BaseEntity/m_pGameSceneNode" );
		static const auto offset = schema::dynamic_field_offset( "client.dll/CGameSceneNode/m_vecAbsOrigin" );

		Member<VClass*>( scene_offset )->Member<vector3d>( offset ) = abs_origin;
	}

	int max_health( ) {
		if ( !util::exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field_offset( "client.dll/C_BaseEntity/m_iMaxHealth" );
		return Member<int>( offset );
	}

	int health( ) {
		if ( !util::exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field_offset( "client.dll/C_BaseEntity/m_iHealth" );
		return Member<int>( offset );
	}

	std::int8_t life_state( ) {
		if ( !util::exists( this ) ) return -1;
		return schema_member<std::int8_t>( "client.dll/C_BaseEntity/m_lifeState" );
	}

	////////////////////

	bool ally( ) {
		if ( !util::exists( this ) || !util::exists( global::g_LocalEntity ) ) return false;
		return reinterpret_cast<C_BaseEntity*>( global::g_LocalEntity )->team_number( ) == this->team_number( );
	}

	CEntityIdentity* identity( ) noexcept {
		if ( !util::exists( this ) ) return nullptr;

		return Member<CEntityIdentity*>( 0x10 );
	}
};