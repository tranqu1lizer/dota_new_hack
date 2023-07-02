#include "panorama_gui.h"
#include "../core/hook/hook.hpp"
#include "../core/features/inventory_changer.h"
#include "../core/features/tree_changer.h"
#include "../core/features/camera_hack.h"

CPanoramaGUI panorama_gui;
extern CheatData context;
void* origCPanel2D__OnMouseButtonDown, * origCPanel2D__OnMouseMove;

bool is_number( const std::string& s ) { return !s.empty( ) && std::all_of( s.begin( ), s.end( ), ::isdigit ); }

void CameraFogCheckbox_Handler( ) {
	features::camera_hack.toggle_fog( );

	panorama_gui.draw_fog ^= true;
}

void ChangerAddItemButton_Handler( ) {
	auto display_text = panorama_gui.changer_item_def_textentry->panel2d_as<CTextEntry>( )->display_text( );
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
	const auto fl_value = panorama_gui.camera_dist_slider->children( )[ 1 ]->panel2d_as<CSlider>( )->g_float( );

	const auto value = static_cast<int>( fl_value * ( features::camera_hack.get_max_distance() - features::camera_hack.get_min_distance( ) ) + features::camera_hack.get_min_distance( ) ); // fl_value * ( max - min ) + min
	features::camera_hack.change_distance( value );
}

void ChangerTreeChanged_Handler( ) {

	CUIPanel* selected;
	if ( selected = panorama_gui.changer_TreesDropDown->panel2d_as<CDropDown>( )->GetSelected( ); !selected )
		return spdlog::critical("ChangerTreeChanged_Handler(): selected = nullptr;\n" );

	const char* dropdown_sel = reinterpret_cast<CLabel*>( selected )->label_text( );
	if ( !dropdown_sel || (std::uintptr_t)dropdown_sel == 0x1 )
		return spdlog::critical( "ChangerTreeChanged_Handler(): dropdown_sel = nullptr;\n" );;

	if ( !util::fast_strcmp( (char*)dropdown_sel, "Default" ) )
		return features::tree_changer.restore_trees( );

	features::tree_changer.change_trees( dropdown_sel );
}

void MiscGoldDisplay_Handler( ) {
	panorama_gui.draw_networthdelta ^= true;
	CUIPanel* disp;

	if ( disp = CPanoramaUIEngine::GetInstance( )->engine_source2( )->find_panel( "DotaHud" )->find_child_traverse( "SpectatorGoldDisplay" ); !disp )
		return;

	disp->panel_style( )->set_visibility( panorama_gui.draw_networthdelta );
}

bool CPanel2D__OnMouseButtonDown( CPanel2D* rcx, const MouseData_t& code ) {
	if ( panorama_gui.menu_status ) {
		if ( code.m_mouse_code == MouseCode::MOUSE_LEFT ) {
			if ( rcx == panorama_gui.camera_fog_checkbox->panel2d_as( ) ) {
				CameraFogCheckbox_Handler( );
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
end:
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

			panorama_gui.main_panel->set_style( std::format( "position: {}% {}% 0;", uixp, uiyp ) );
		}
	}
}

void CPanoramaGUI::register_events( ) {
	auto ui_engine = CPanoramaUIEngine::GetInstance( )->engine_source2( );
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
		ui_engine->register_event_handler_client( "SliderValueChanged", camera_dist_slider, camera_dist_handler );
		ui_engine->register_event_handler_client( "DropDownSelectionChanged", changer_TreesDropDown, changer_tree_handler );
	}
	else {
		// hook::uninstall_hook( aOnMouseButtonDown );
		ui_engine->unregister_event_handler( "SliderValueChanged", camera_dist_slider, camera_dist_handler );
		ui_engine->unregister_event_handler( "DropDownSelectionChanged", changer_TreesDropDown, changer_tree_handler );
	}
}

void CPanoramaGUI::show( ) {
	auto ui_engine = CPanoramaUIEngine::GetInstance( );

	if ( CUIPanel* root = ui_engine->engine_source2( )->find_panel( global::in_game ? "DotaHud" : "DotaDashboard" ); root ) {

		if ( !main_panel ) {
			main_panel = ui_engine->engine_source2( )->create_panel( "mainmenu", root )->ui_panel( );
			if ( ui_engine->engine_source2( )->is_valid_panel_ptr( main_panel ) ) {
				main_panel->load_layout_file( "file://{resources}/menu.xml", false );
			}
			camera_dist_slider = main_panel->find_child_traverse( "camera_distance_slider" );
			camera_fog_checkbox = main_panel->find_child_traverse( "camera_draw_fog" );
			camera_draw_button = main_panel->find_child_traverse( "camera_draw_particles" );
			changer_item_def_textentry = main_panel->find_child_traverse( "item_def_to_add" );
			changer_add_item_button = main_panel->find_child_traverse( "add_item" );
			visuals_hpnumb = main_panel->find_child_traverse( "visual_health_number" );
			visuals_manabar = main_panel->find_child_traverse( "visual_mana_bar" );
			misc_log_searching_players = main_panel->find_child_traverse( "misc_log_searching_players" );
			misc_autoaccept = main_panel->find_child_traverse( "misc_autoaccept" );
			misc_networth_delta = main_panel->find_child_traverse( "misc_networth_delta" );
			misc_unlock_emoticons = main_panel->find_child_traverse( "misc_unlock_emoticons" );
			changer_TreesDropDown = main_panel->find_child_traverse( "TreesDropDown" );

			main_panel->find_child_traverse( "GeneralTab" )->panel_style( )->set_visibility( true );
			main_panel->find_child_traverse( "ChangerTab" )->panel_style( )->set_visibility( false );
			main_panel->find_child_traverse( "MiscTab" )->panel_style( )->set_visibility( false );
			camera_draw_button->panel2d_as<CDOTA_DB_SettingsCheckbox>( )->set_selected( draw_particles );
		}

		main_panel->set_parent( root );
		main_parent_panel = root;
		main_panel->panel_style( )->set_visibility( menu_status ^= true );
		register_events( );
		ui_engine->engine_source2( )->play_sound_effect( "ui_menu_activate_open" );
	}
}