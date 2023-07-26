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
	auto& fog_controller = context.entities[ "env_fog_controller" ];
	if ( fog_controller.empty( ) )
		return;

	C_FogController* fog = (C_FogController*)fog_controller.front( );

	if ( !fog || !fog->GetFogParams( ) )
		return;

	fog->GetFogParams( )->enable ^= true;
}