#pragma once

#include "C_BaseCombatCharacter.hpp"
#include "C_FogController.h"
#include "color.hpp"

struct C_fogplayerparams_t
{
private:
	void* pd; // 0x0
public:
	CHandle m_hCtrl; // 0x8	
	float m_flTransitionTime; // 0xc	
	Color m_OldColor; // 0x10	
	float m_flOldStart; // 0x14	
	float m_flOldEnd; // 0x18	
	float m_flOldMaxDensity; // 0x1c	
	float m_flOldHDRColorScale; // 0x20	
	float m_flOldFarZ; // 0x24	
	Color m_NewColor; // 0x28	
	float m_flNewStart; // 0x2c	
	float m_flNewEnd; // 0x30	
	float m_flNewMaxDensity; // 0x34	
	float m_flNewHDRColorScale; // 0x38	
	float m_flNewFarZ; // 0x3c	
};

class CPlayer_CameraServices : NormalClass
{
public:
	auto player_fog( ) {
		const auto offset = schema::dynamic_field_offset( "client.dll/CPlayer_CameraServices/m_PlayerFog" );

		return MemberNotPtr<C_fogplayerparams_t>( offset );
	}
};

class C_BasePlayerPawn : public C_BaseCombatCharacter
{
public:
	CPlayer_CameraServices* camera_services( ) {
		return schema_member<CPlayer_CameraServices*>( "client.dll/C_BasePlayerPawn/m_pCameraServices" );
	}
};