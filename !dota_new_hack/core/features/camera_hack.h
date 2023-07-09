#pragma once

#include "../../main.hpp"

class CCameraHack {
	float m_current_distance = 1200.f, m_dist_step = 100.f;

	const float m_min_dist_ = 1000.f, m_max_dist_ = 4000.f;
public:
	auto get_distance( ) { return m_current_distance; }
	auto get_min_distance( ) { return m_min_dist_; }
	auto get_max_distance( ) { return m_max_dist_; }
	float change_distance( const float dist = 1200.f );
	void on_mouse_wheeled( CDOTA_Camera*, int );
	void toggle_fog( );
};


namespace features {
	inline CCameraHack camera_hack{};
}