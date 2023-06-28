#pragma once

#include <span>
#include "vector.hpp"
#include "NormalClass.hpp"

struct C_DOTA_TrackingProjectileInfo
{
    int32_t m_iHandle; // 0x0
    int32_t m_iMoveSpeed; // 0x4
    vector3d m_vLocation; // 0x8
    uint32_t m_hSource; // 0x14
    uint32_t m_hTarget; // 0x18
    vector3d m_vTargetLocation; // 0x1c
    bool m_bDodgeable; // 0x28
    bool m_bIsAttack; // 0x29
    bool m_bIsEvaded; // 0x2a
private:
    uint8_t pad_0x2b[1]; // 0x2b
public:
    float m_flExpireTime; // 0x2c
    float m_flMaxImpactTime; // 0x30
private:
    uint8_t pad_0x34[36]; // 0x34
};

// Aligment: 10
// Size: 144
struct C_DOTA_LinearProjectileInfo
{
private:
    uint8_t pad_0x0[32]; // 0x0
public:
    uint8_t m_transform[32]; // 0x20
    int32_t m_iHandle; // 0x40
    vector3d m_vInitPosition; // 0x44
    vector3d m_vPosition; // 0x50
    vector3d m_vVelocity; // 0x5c
    vector3d m_vAcceleration; // 0x68
    float m_flMaxSpeed; // 0x74
    float m_flFowRadius; // 0x78
    bool m_bStickyFoWReveal; // 0x7c
private:
    uint8_t pad_0x7d[3]; // 0x7d
public:
    float m_flDistance; // 0x80
private:
    uint8_t pad_0x84[12]; // 0x84
};

class C_DOTA_ProjectileManager {
	char _pad[0x30];
public:
    static C_DOTA_ProjectileManager* GetInstance( ) {
        return (C_DOTA_ProjectileManager*)util::find_game_system( "C_DOTA_ProjectileManager" );
    }

	C_DOTA_TrackingProjectileInfo* m_pTrackingProjectiles[1024];
	C_DOTA_LinearProjectileInfo* m_pLinearProjectiles[1024];
};