#pragma once

#include "C_BaseEntity.hpp"
#include "NormalClass.hpp"

// Alignment: 25
// Size: 0x68
struct fogparams_t
{
private:
	[[maybe_unused]] uint8_t __pad0000[ 0x8 ]; // 0x0
public:
	vector3d dirPrimary; // 0x8	
	Color colorPrimary; // 0x14	
	Color colorSecondary; // 0x18	
	Color colorPrimaryLerpTo; // 0x1c	
	Color colorSecondaryLerpTo; // 0x20	
	float start; // 0x24	
	float end; // 0x28	
	float farz; // 0x2c	
	float maxdensity; // 0x30	
	float exponent; // 0x34	
	float HDRColorScale; // 0x38	
	float skyboxFogFactor; // 0x3c	
	float skyboxFogFactorLerpTo; // 0x40	
	float startLerpTo; // 0x44	
	float endLerpTo; // 0x48	
	float maxdensityLerpTo; // 0x4c	
	float lerptime; // 0x50	
	float duration; // 0x54	
	float blendtobackground; // 0x58	
	float scattering; // 0x5c	
	float locallightscale; // 0x60	
	bool enable; // 0x64	
	bool blend; // 0x65	
	bool m_bNoReflectionFog; // 0x66	
	bool m_bPadding; // 0x67	
};

class C_FogController : public C_BaseEntity {
public:
	OFFSET_INLINE( fogparams_t, GetFogParams, 0x528 );
};