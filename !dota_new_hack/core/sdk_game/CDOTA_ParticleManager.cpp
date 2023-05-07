#include "CDOTA_ParticleManager.hpp"

int CDOTA_ParticleManager::GetParticleCount( ) {
	return Member<uint32_t>( 0x80 );
}

CDOTA_ParticleManager::ParticleContainer** CDOTA_ParticleManager::GetParticleArray( ) {
	return Member<ParticleContainer**>( 0x88 );
}

uint32_t CDOTA_ParticleManager::GetHandle( ) {
	return Member<uint32_t>( 0xb8 );
}

void CDOTA_ParticleManager::IncHandle( ) {
	*(uint32_t*)( (uintptr_t)this + 0xb8 ) = GetHandle( ) + 1;
}

std::uint32_t CDOTA_ParticleManager::CreateParticle( const char* name, ParticleAttachment_t attachType, C_BaseEntity* ent ) {
	static_assert( sizeof( CreateParticleInfo ) == 0x40 );
	CreateParticleInfo info{};
	info.m_szParticleName = name;
	info.m_particleAttachment = attachType;
	info.m_pTargetEntity = ent;

	auto h = GetHandle( );
	IncHandle( );
	CallVFunc<9>( h, &info );

	ParticleWrapper result{};
	result.info = info;
	result.particle = GetParticleArray( )[GetParticleCount( ) - 1]->GetParticle( );
	result.handle = h;

	particles.push_back( result );

	return result.handle;
}

void CDOTA_ParticleManager::DestroyParticle( uint32_t handle ) {
	functions_call::DestroyParticle( this, handle, 1 );
}

void CDOTA_ParticleManager::DestroyParticle( const char* name, C_BaseEntity* entity ) {
	for ( auto& particle : particles ) {
		if ( particle.info.m_szParticleName == name && particle.info.m_pTargetEntity == entity ) {
			DestroyParticle( particle );
		}
	}
}

void CDOTA_ParticleManager::DestroyParticle( ParticleWrapper& particleWrap ) {
	if ( particleWrap.handle == std::numeric_limits<std::uint32_t>::max( ) ) return;
	functions_call::DestroyParticle( this, particleWrap.handle, 1 );
	particleWrap.Invalidate( );
}

void CDOTA_ParticleManager::DestroyAllParticles( ) {
	for ( auto& pw : particles )
		DestroyParticle( pw );

	particles.clear( );
}