#pragma once

#include "../../main.hpp"

struct ParticleFilterType_t;
struct ParticleFunctionType_t;

class IParticleSnapshot;
class IParticleSystemQuery;
class ISceneWorld;
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

	virtual CSchemaClassBinding* Schema_DynamicBinding( void ) = 0;
	virtual void* GetOwner( void ) = 0;
	virtual void SetAssociatedObj( void* ) = 0;
	virtual void* GetAssociatedObj( ) = 0;
	virtual void SetSomethingElse( void* ) = 0;
	virtual void* GetSomethingElse( ) = 0;
	virtual bool IsValid( void ) = 0;
	virtual void* unk1( void ) = 0;
	virtual void* GetDefinition( void ) = 0;
	virtual C_ParticleCollection* GetParentCollection( void ) = 0;
	virtual C_ParticleCollection* GetFirstChildCollection( void ) = 0;
	virtual C_ParticleCollection* GetNextSiblingCollection( void ) = 0;
	virtual C_ParticleCollection* GetNextCollectionUsingSameDef( void ) = 0;
	virtual bool UsesPowerOfTwoFrameBufferTexture( bool ) = 0;
	virtual void sub_105ED0( ) = 0;
	virtual void sub_1062B0( ) = 0;
	virtual void* SetParticleControl( int control_point_idx, vector3d& );
	virtual void SetOrientationFLU( ) = 0;
	virtual void sub_105C60( ) = 0;
	virtual void sub_107C90( ) = 0;
	virtual void sub_1073F0( ) = 0;
	virtual vector3d* GetControlPointPosition( int ControlPoint ) = 0; // 21
	virtual void sub_105861( ) = 0;
	virtual void* GetControlPointObject( int ControlPoint ) = 0;
};


class CNewParticleEffect : VClass {
public:
	C_ParticleCollection* GetParticleCollection( ) {
		return Member<C_ParticleCollection*>( 0x20 );
	}

	C_BaseEntity* GetEntityForParticle( ) {
		return Member<C_BaseEntity*>( Member<uintptr_t>( 0x58 ) + 0x170 );
	}

	CNewParticleEffect* SetControlPoint( int idx, const vector3d& pos ) {
		auto collection = GetParticleCollection( );
		collection->CallVFunc<16>( idx, &pos );
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

	void Invalidate( ) {
		particle = nullptr;
		handle = std::numeric_limits<std::uint32_t>::max( );
		info = CreateParticleInfo{};
	}
};

class CDOTA_ParticleManager : public VClass {
	static inline std::vector<ParticleWrapper> particles{};
	static auto GetInstanceImpl( )
	{
		return *reinterpret_cast<CDOTA_ParticleManager**>( global::patterns::DOTAParticleManager );
	}
public:
	static auto& GetInstance( )
	{
		static CDOTA_ParticleManager& instance = *GetInstanceImpl( );
		return instance;
	}

	struct ParticleContainer : NormalClass {
		inline CNewParticleEffect* GetParticle( ) {
			return Member<CNewParticleEffect*>( 0x10 );
		}
	};
	int GetParticleCount( );
	ParticleContainer** GetParticleArray( );

	uint32_t GetHandle( );
	void IncHandle( );

	std::uint32_t CreateParticle( const char* name, ParticleAttachment_t attachType, C_BaseEntity* ent );
	void DestroyParticle( const char* name, C_BaseEntity* entity );
	void DestroyParticle( uint32_t handle );
	void DestroyParticle( ParticleWrapper& info );
	void DestroyAllParticles( );
};