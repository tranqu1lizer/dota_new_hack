#pragma once

#include "../../main.hpp"

struct ParticleFilterType_t;
struct ParticleFunctionType_t;

class IParticleSnapshot;
class IParticleSystemQuery;
class CNewParticleEffect;
class CParticleSystemDefinition;
class Particle;

enum ParticleAttachment_t : int
{
	PATTACH_INVALID = -1,
	PATTACH_ABSORIGIN = 0,
	PATTACH_ABSORIGIN_FOLLOW,
	PATTACH_CUSTOMORIGIN,
	PATTACH_CUSTOMORIGIN_FOLLOW,
	PATTACH_POINT,
	PATTACH_POINT_FOLLOW,
	PATTACH_EYES_FOLLOW,
	PATTACH_OVERHEAD_FOLLOW,
	PATTACH_WORLDORIGIN,
	PATTACH_ROOTBONE_FOLLOW,
	PATTACH_RENDERORIGIN_FOLLOW,
	PATTACH_MAIN_VIEW,
	PATTACH_WATERWAKE,
	PATTACH_CENTER_FOLLOW,
	PATTACH_CUSTOM_GAME_STATE_1,
	PATTACH_HEALTHBAR,
	MAX_PATTACH_TYPES
};

struct InfoStruct
{
	const char* name;
	//....
};

struct WeakHandleContents_InfoForResourceTypeIParticleSystemDefinition
{
	CParticleSystemDefinition* systemDefinition;
	InfoStruct* info;
	char _pad[0x200];
};

struct CWeakHandle_InfoForResourceTypeIParticleSystemDefinition
{
	WeakHandleContents_InfoForResourceTypeIParticleSystemDefinition* contents;
};

class C_ParticleCollection : public VClass {
public:
	CNewParticleEffect* GetNewPacticle( ) {
		return Member<CNewParticleEffect*>( 0x130 );
	}
	void SetControlPoint( int ControlPoint, vector3d& cp ) {
		CallVFunc<16, void>( ControlPoint, cp );
	}

	vector3d* GetControlPointPosition( int ControlPoint ) {
		return CallVFunc<22, vector3d*>( ControlPoint );
	}

	void SetDormant( bool state ) {
		CallVFunc<63, void>( state );
	}

	bool GetRenderingEnabled( ) {
		return CallVFunc<91, bool>( );
	}

	void SetRenderingEnabled( bool state ) {
		CallVFunc<92, void>( state );
	}
};

class CNewParticleEffect : VClass {
public:
	char pad_0000[32]; // 0x0
	C_ParticleCollection* m_pCollection; // 0x20
	char pad_0028[32]; // 0x28
	uint8_t m_bDeleteFlag; // 0x48

	C_BaseEntity* GetEntityForParticle( ) {
		return Member<C_BaseEntity*>( Member<uintptr_t>( 0x58 ) + 0x170 );
	}

	CNewParticleEffect* SetControlPoint( int idx, const vector3d& pos ) {
		if ( !m_pCollection )
			return nullptr;

		m_pCollection->CallVFunc<16>( idx, &pos );
		return this;
	}
};

struct CreateParticleInfo
{
	const char* m_szParticleName;
	ParticleAttachment_t m_particleAttachment;
	C_BaseEntity* m_pTargetEntity;
private:
	void* unk[5] = {
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr
	};
};

struct ParticleWrapper {
	CreateParticleInfo info{};
	CNewParticleEffect* particle{};
	std::uint32_t handle{};

	void invalidate( ) {
		particle = nullptr;
		handle = std::numeric_limits<std::uint32_t>::max( );
		info = CreateParticleInfo{};
	}
};

class CDOTA_ParticleManager : public VClass {
public:
	static inline std::vector<ParticleWrapper> particles{};

	static CDOTA_ParticleManager* GetInstance( )
	{
		return *reinterpret_cast<CDOTA_ParticleManager**>( global::patterns::DOTAParticleManager );
	}

	struct ParticleContainer : NormalClass {
		inline CNewParticleEffect* GetParticle( ) {
			return Member<CNewParticleEffect*>( 0x10 );
		}
	};
	GETTER( CUtlVector<ParticleContainer*>, GetParticles, 0x80 );
	FIELD( uint32_t, handle, 0xb8 );

	ParticleWrapper create_particle( const char* name, ParticleAttachment_t attachType, C_BaseEntity* ent );
	void destroy_particle( const char* name, C_BaseEntity* entity );
	void destroy_particle( uint32_t handle );
	void destroy_particle( ParticleWrapper& info );
	void destroy_own_particles( );
};