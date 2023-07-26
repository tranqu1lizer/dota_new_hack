#pragma once

#include "CDOTA_ModifierManager.hpp"
#include "CHandle.hpp"
#include "CEntityIdentity.hpp"
#include "CGameEntitySystem.hpp"

#include "NormalClass.hpp"
#include "ISchemaSystem.hpp"
#include "CSource2Client.hpp"
#include "../util/util.hpp"
#include "../schema.h"

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

class C_BaseEntity : public VClass {
public:
	OFFSET( CEntityIdentity*, GetIdentity, 0x10 );
	OFFSET( int, GetMaxHealth, schema::C_BaseEntity::m_iMaxHealth );
	OFFSET( int, GetHealth, schema::C_BaseEntity::m_iHealth );
	OFFSET( std::int8_t, GetLifeState, schema::C_BaseEntity::m_lifeState );
	OFFSET( CHandle, GetOwner, schema::C_BaseEntity::m_hOwnerEntity );
	OFFSET( DOTATeam_t, GetTeamNumber, schema::C_BaseEntity::m_iTeamNum );

	CSchemaClassBinding* Schema_DynamicBinding( ) {
		return CallVFunc<0, CSchemaClassBinding*>( );
	}

	ClientClass* GetClientClass( ) {
		return CallVFunc<33, ClientClass*>( );
	}

	int GetIndex( ) {
		if ( !util::exists( this ) ) return -1;
		return GetIdentity( )->entHandle & 0x7FFF;
	}

	vector3d GetAbsoluteOrigin( )
	{
		if ( !util::exists( this ) ) return vector3d{ -1,-1,-1 };

		const auto scene = *(uintptr_t*)( (uintptr_t)this + schema::C_BaseEntity::m_pGameSceneNode );

		return *(vector3d*)( scene + schema::CGameSceneNode::m_vecAbsOrigin );
	}

	void SetAbsOrigin( vector3d abs_origin ) {
		if ( !util::exists( this ) ) return;

		Member<VClass*>( schema::C_BaseEntity::m_pGameSceneNode )->Member<vector3d>( schema::CGameSceneNode::m_vecAbsOrigin ) = abs_origin;
	}

	bool IsAllyWith( C_BaseEntity* w ) {
		if ( !util::exists( this ) || !util::exists( w ) ) return false;
		return w->GetTeamNumber( ) == this->GetTeamNumber( );
	}
};