#pragma once

#include "../global.hpp"

class CBasePlayerController : public C_BaseEntity {
public:
	OFFSET( CHandle, GetPawn, schema::CBasePlayerController::m_hPawn );
	OFFSET( bool, IsLocalPlayer, schema::CBasePlayerController::m_bIsLocalPlayerController );
};

class C_DOTAPlayerController : public CBasePlayerController {
public:
	OFFSET( CHandle, GetAssignedHero, schema::C_DOTAPlayerController::m_hAssignedHero );
};