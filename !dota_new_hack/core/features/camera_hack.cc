#include "camera_hack.h"

float CCameraHack::change_distance( const float dist ) {
	m_current_distance = dist;
	if ( auto camera = calls::GetCurrentCamera( ); camera ) camera->set_distance( m_current_distance );

	return dist;
}

void CCameraHack::on_mouse_wheeled( CDOTA_Camera* cam, int delta ) {
	if ( delta == -1 ) m_current_distance += m_dist_step;
	else if ( delta == 1 ) m_current_distance -= m_dist_step;
	if ( m_current_distance > m_max_dist_ ) m_current_distance = m_max_dist_;
	if ( m_current_distance < m_min_dist_ ) m_current_distance = m_min_dist_;

	features::camera_hack.change_distance( m_current_distance );
}

void CCameraHack::toggle_fog( ) {
	if ( auto camera = reinterpret_cast<std::uintptr_t>( calls::GetCurrentCamera( ) ); camera ) {
		const auto get_fog_vfunc = util::vmt( camera, 18 ); // C_DOTACamera::GetFogEnd
		const auto instruction_bytes = *reinterpret_cast<std::uintptr_t*>( get_fog_vfunc );

		if ( instruction_bytes == 0x83485708245c8948 ) { // not patched

			// 0x0F, 0x57, 0xC0 | xorps xmm0, xmm0
			// 0xC3				| ret
			constexpr const char* bytepatch = "\x0F\x57\xC0\xC3";
			util::patch( (void*)get_fog_vfunc, bytepatch );
		}
		else if ( instruction_bytes == 0x83485708c3c0570f ) { // already patched

			// 0x48, 0x89, 0x5C, 0x24, 0x08 | mov qword ptr ss:[rsp+8], rbx
			// 0x57							| push rdi
			constexpr const char* byterestore = "\x48\x89\x5C\x24\x08\x57";
			util::patch( (void*)get_fog_vfunc, byterestore );
		}
	}
}