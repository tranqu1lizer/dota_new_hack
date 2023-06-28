#include "CDOTA_ParticleManager.hpp"


ParticleWrapper CDOTA_ParticleManager::create_particle( const char* name, ParticleAttachment_t attachType, C_BaseEntity* ent ) {
	CreateParticleInfo info{};
	info.m_szParticleName = name;
	info.m_particleAttachment = attachType;
	info.m_pTargetEntity = ent;

	CallVFunc<9>( handle( ), &info );

	ParticleWrapper result{};
	result.info = info;
	result.particle = GetParticles( ).last()->GetParticle( );
	result.handle = handle( )++;

	particles.push_back( result );

	return result;
}

void CDOTA_ParticleManager::destroy_particle( uint32_t handle ) {
	calls::destroy_particle( this, handle, 1 );
}

void CDOTA_ParticleManager::destroy_particle( const char* name, C_BaseEntity* entity ) {
	for ( auto& particle : particles ) {
		if ( particle.info.m_szParticleName == name && particle.info.m_pTargetEntity == entity ) {
			destroy_particle( particle );
		}
	}
}

void CDOTA_ParticleManager::destroy_particle( ParticleWrapper& particleWrap ) {
	if ( particleWrap.handle == std::numeric_limits<std::uint32_t>::max( ) ) return;
	calls::destroy_particle( this, particleWrap.handle, 1 );
	particleWrap.invalidate( );
}

void CDOTA_ParticleManager::destroy_own_particles( ) {
	for ( auto& pw : particles )
		destroy_particle( pw );

	particles.clear( );
}