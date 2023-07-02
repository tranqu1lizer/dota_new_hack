#include "hero_bar.h"

bool CHeroBar::can_draw_for( C_DOTA_BaseNPC_Hero* h ) {
	if ( !h ||
		 h->ally( ) ||
		 h->identity( )->dormant( ) ||
		 h->life_state( ) != 0 ||
		 h->illusion( )
		 )
		return false;

	return true;
}

void CHeroBar::draw_health( const bool off ) {
	static constexpr auto font_size = 17;
	for ( auto& hero : context.heroes ) {
		if ( !can_draw_for( hero ) )
			continue;

		vector2d scr;
		vector3d pos = hero->abs_origin( );
		pos.z += hero->health_bar_offset( );

		if ( !CRenderGameSystem::GetInstance( )->GetVectorInScreenSpace( pos, scr ) )
			continue;

		off ? scr.y -= 43 : scr.y -= 39;

		drawing::DrawTextForeground( DrawData.GetFont( "Monofonto", font_size ), std::to_string( hero->health( ) ), ImVec2{ scr.x, scr.y }, font_size, ImColor{ 255,255,255 }, true );
	}
}

void CHeroBar::draw_mana( ) {
	constexpr static ImVec2 manabarSize{ 138, 12 };
	for ( auto& hero : context.heroes ) {
		if ( !can_draw_for( hero ) )
			continue;

		vector2d scr;
		vector3d pos = hero->abs_origin( );
		pos.z += hero->health_bar_offset( );

		if ( !CRenderGameSystem::GetInstance( )->GetVectorInScreenSpace( pos, scr ) )
			continue;

		const ImVec2 drawPos{ scr.x - 15, scr.y - 16 };

		// Background
		drawing::DrawRectFilled(
			drawPos - ImVec2( 110, manabarSize.y ) / 2,
			manabarSize, ImVec4( 0, 0, 0, 1 ) );
		// Manabar
		drawing::DrawRectFilled(
			drawPos - ImVec2( 110, manabarSize.y ) / 2 + ImVec2( 1, 1 ),
			ImVec2( manabarSize.x * ( hero->mana( ) / hero->max_mana( ) ) - 2, manabarSize.y - 2 ), ImVec4( 0, 0.5, 1, 1 ) );
	}
}