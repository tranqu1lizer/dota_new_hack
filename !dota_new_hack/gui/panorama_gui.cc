#include "panorama_gui.h"
#include "../core/hook/hook.hpp"
#include "../core/features/inventory_changer.h"
#include "../core/features/tree_changer.h"
#include "../core/features/camera_hack.h"

CPanoramaGUI panorama_gui;
extern CContext context;
void* origCPanel2D__OnMouseButtonDown, * origCPanel2D__OnMouseMove;

bool is_number( const std::string& s ) { return !s.empty( ) && std::all_of( s.begin( ), s.end( ), ::isdigit ); }

void ChangerAddItemButton_Handler( ) {
	auto display_text = panorama_gui.changer_item_def_textentry->panel2d_as<CTextEntry>( )->GetDisplayText( );
	if ( !display_text.empty( ) && is_number( display_text ) ) {
		if ( const auto def_index = std::stoi( display_text ); def_index ) {
			features::inventory_changer.create_fake_item( def_index );
		}
	}
}

void VisualsManabarButton_Handler( ) {
	static void* sig = (void*)util::find_pattern( global::client, "\x75\xCC\xC6\x44\x24\xCC\xCC\xEB\xCC\xC6\x44\x24\xCC\xCC\xBA", "", false );
	panorama_gui.draw_mana_bar ? util::patch( sig, "\x75\x07" ) : util::patch( sig, "\xEB\x07" );

	panorama_gui.draw_mana_bar ^= true;
}

void CameraDistSlider_Handler( ) {
	const auto fl_value = panorama_gui.camera_dist_slider->GetChildren( )[ 1 ]->panel2d_as<CSlider>( )->GetFloat( );

	const auto value = static_cast<int>( fl_value * ( features::camera_hack.get_max_distance( ) - features::camera_hack.get_min_distance( ) ) + features::camera_hack.get_min_distance( ) ); // fl_value * ( max - min ) + min
	features::camera_hack.change_distance( value );
}

void ChangerTreeChanged_Handler( ) {

	CUIPanel* selected;
	if ( selected = panorama_gui.changer_TreesDropDown->panel2d_as<CDropDown>( )->GetSelected( ); !selected )
		return spdlog::critical( "{}(): selected = nullptr;\n", __FUNCTION__ );

	const char* dropdown_sel = reinterpret_cast<CLabel*>( selected )->GetLabelText( );
	if ( !dropdown_sel || (std::uintptr_t)dropdown_sel == 0x1 )
		return spdlog::critical( "{}(): dropdown_sel = nullptr;\n", __FUNCTION__ );

	if ( !util::fast_strcmp( dropdown_sel, "Default" ) )
		return features::tree_changer.restore_trees( );

	features::tree_changer.change_trees( dropdown_sel );
}

void MiscGoldDisplay_Handler( ) {
	panorama_gui.draw_networthdelta ^= true;
	CUIPanel* disp;

	if ( disp = context.DotaHud->FindChildTraverse( "SpectatorGoldDisplay" ); !disp )
		return;

	disp->SetActive( panorama_gui.draw_networthdelta );
}

bool CPanel2D__OnMouseButtonDown( CPanel2D* rcx, const MouseData_t& code ) {
	if ( panorama_gui.menu_status ) {
		if ( code.m_mouse_code == MouseCode::MOUSE_LEFT ) {
			if ( rcx == panorama_gui.camera_fog_checkbox->panel2d_as( ) ) {
				features::camera_hack.toggle_fog( );
				panorama_gui.fog_enabled ^= true;
			}
			else if ( rcx == panorama_gui.changer_add_item_button->panel2d_as( ) ) {
				ChangerAddItemButton_Handler( );
			}
			else if ( rcx == panorama_gui.visuals_manabar->panel2d_as( ) ) {
				VisualsManabarButton_Handler( );
			}
			else if ( rcx == panorama_gui.visuals_hpnumb->panel2d_as( ) )
				panorama_gui.draw_health ^= true;
			else if ( rcx == panorama_gui.misc_log_searching_players->panel2d_as( ) )
				panorama_gui.log_searching_players ^= true;
			else if ( rcx == panorama_gui.misc_autoaccept->panel2d_as( ) )
				panorama_gui.auto_accept ^= true;
			else if ( rcx == panorama_gui.camera_draw_button->panel2d_as( ) )
				panorama_gui.draw_particles ^= true;
			else if ( rcx == panorama_gui.misc_unlock_emoticons->panel2d_as( ) )
				ICVar::get( )[ "dota_hud_chat_enable_all_emoticons" ]->m_values.boolean ^= true;
			else if ( rcx == panorama_gui.misc_networth_delta->panel2d_as( ) ) {
				MiscGoldDisplay_Handler( );
			}
		}
	}

	return reinterpret_cast<decltype( &CPanel2D__OnMouseButtonDown )>( origCPanel2D__OnMouseButtonDown )( rcx, code );
}

void CPanel2D__OnMouseMove( CPanel2D* rcx, float flMouseX, float flMouseY ) {
	reinterpret_cast<decltype( &CPanel2D__OnMouseMove )>( origCPanel2D__OnMouseMove )( rcx, flMouseX, flMouseY );

	if ( rcx == panorama_gui.main_panel->panel2d_as( ) ) {
		if ( GetKeyState( VK_LBUTTON ) & 0x8000 ) {

			const auto uiw = panorama_gui.main_parent_panel->actuallayoutwidth( );
			const auto uih = panorama_gui.main_parent_panel->actuallayoutheight( );
			POINT p;

			if ( !GetCursorPos( &p ) )
				return;

			const auto uixp = p.x / uiw * 100;
			const auto uiyp = p.y / uih * 100;

			panorama_gui.main_panel->SetStyle( std::format( "position: {}% {}% 0;", uixp, uiyp ) );
		}
	}
}

void CPanoramaGUI::register_events( ) {
	auto ui_engine = CPanoramaUIEngine::get( )->AccessUIEngine( );
	FastDelegate0<void> camera_dist_handler( CameraDistSlider_Handler );
	FastDelegate0<void> changer_tree_handler( ChangerTreeChanged_Handler );

	static constexpr auto OnMouseButtonDown_idx = 14;
	static constexpr auto OnMouseMove_idx = 19;
	const auto aOnMouseButtonDown = main_panel->panel2d_as( )->GetVF( OnMouseButtonDown_idx );
	const auto aOnMouseMove = main_panel->panel2d_as( )->GetVF( OnMouseMove_idx );

	if ( menu_status ) {

		if ( !hook::hooks.contains( aOnMouseButtonDown ) || !hook::hooks.contains( aOnMouseMove ) ) {
			hook::install_hook( aOnMouseButtonDown, &CPanel2D__OnMouseButtonDown, &origCPanel2D__OnMouseButtonDown, "CPanel2D::OnMouseButtonDown" );
			hook::install_hook( aOnMouseMove, &CPanel2D__OnMouseMove, &origCPanel2D__OnMouseMove, "CPanel2D::OnMouseMove" );
		}
		ui_engine->RegisterEventHandlerClient( "SliderValueChanged", camera_dist_slider, camera_dist_handler );
		ui_engine->RegisterEventHandlerClient( "DropDownSelectionChanged", changer_TreesDropDown, changer_tree_handler );
	}
	else {
		// hook::uninstall_hook( aOnMouseButtonDown );
		ui_engine->UnregisterEventHandler( "SliderValueChanged", camera_dist_slider, camera_dist_handler );
		ui_engine->UnregisterEventHandler( "DropDownSelectionChanged", changer_TreesDropDown, changer_tree_handler );
	}
}

void CPanoramaGUI::show( ) {
	auto ui_engine = CPanoramaUIEngine::get( );

	if ( CUIPanel* root = ui_engine->AccessUIEngine( )->FindPanel( IEngineClient::get( ).IsInGame( ) ? "DotaHud" : "DotaDashboard" ); root ) {

		if ( !main_panel ) {
			main_panel = ui_engine->AccessUIEngine( )->CreatePanel( "mainmenu", root )->UIPanel( );
			if ( ui_engine->AccessUIEngine( )->IsValidPanelPtr( main_panel ) ) {
				main_panel->LoadLayoutFile( "file://{resources}/menu.xml", false );
			}
			camera_dist_slider = main_panel->FindChildTraverse( "camera_distance_slider" );
			camera_fog_checkbox = main_panel->FindChildTraverse( "camera_draw_fog" );
			camera_draw_button = main_panel->FindChildTraverse( "camera_draw_particles" );
			changer_item_def_textentry = main_panel->FindChildTraverse( "item_def_to_add" );
			changer_add_item_button = main_panel->FindChildTraverse( "add_item" );
			visuals_hpnumb = main_panel->FindChildTraverse( "visual_health_number" );
			visuals_manabar = main_panel->FindChildTraverse( "visual_mana_bar" );
			misc_log_searching_players = main_panel->FindChildTraverse( "misc_log_searching_players" );
			misc_autoaccept = main_panel->FindChildTraverse( "misc_autoaccept" );
			misc_networth_delta = main_panel->FindChildTraverse( "misc_networth_delta" );
			misc_unlock_emoticons = main_panel->FindChildTraverse( "misc_unlock_emoticons" );
			changer_TreesDropDown = main_panel->FindChildTraverse( "TreesDropDown" );

			main_panel->FindChildTraverse( "GeneralTab" )->GetStyle( )->SetVisibility( true );
			main_panel->FindChildTraverse( "ChangerTab" )->GetStyle( )->SetVisibility( false );
			main_panel->FindChildTraverse( "MiscTab" )->GetStyle( )->SetVisibility( false );
			camera_draw_button->panel2d_as<CDOTA_DB_SettingsCheckbox>( )->SetSelected( draw_particles );
		}

		main_panel->SetParent( root );
		main_parent_panel = root;
		main_panel->GetStyle( )->SetVisibility( menu_status ^= true );
		register_events( );
		ui_engine->AccessUIEngine( )->RunScript(
			"Hud",
			"$.DispatchEvent( 'PlaySoundEffect', 'ui_menu_activate_open' );",
			IEngineClient::get( ).IsInGame( ) ? "panorama/layout/base_hud.xml" : "panorama/layout/base.xml"
		);
	}
}