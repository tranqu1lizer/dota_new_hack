#include "panorama_gui.h"
#include "../core/hook/hook.hpp"
#include "../core/features/inventory_changer.h"

CPanoramaGUI panorama_gui;
extern CheatData cheat_data;
void* origCPanel2D__OnMouseButtonDown, * origCPanel2D__OnMouseMove;

bool is_number( const std::string& s ) {
	return !s.empty( ) && std::all_of( s.begin( ), s.end( ), ::isdigit );
}

void CameraFogCheckbox_Handler( ) {
	if ( auto camera = reinterpret_cast<VClass*>( calls::GetCurrentCamera( ) ); camera ) {
		const auto get_fog_vfunc = camera->GetVF( 18 ); // C_DOTACamera::GetFogEnd
		const auto instruction_bytes = *reinterpret_cast<std::uintptr_t*>( get_fog_vfunc );

		if ( instruction_bytes == 0x83485708245c8948 ) { // not patched

			// 0x0F, 0x57, 0xC0 | xorps xmm0, xmm0
			// 0xC3				| ret
			constexpr const char* bytepatch = "\x0F\x57\xC0\xC3";
			util::patch( (void*)get_fog_vfunc, bytepatch, 4 );
		}
		else if ( instruction_bytes == 0x83485708c3c0570f ) { // already patched

			// 0x48, 0x89, 0x5C, 0x24, 0x08 | mov qword ptr ss:[rsp+8], rbx
			// 0x57							| push rdi
			constexpr const char* byterestore = "\x48\x89\x5C\x24\x08\x57";
			util::patch( (void*)get_fog_vfunc, byterestore, 6 );
		}
	}

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

	const auto value = static_cast<int>( fl_value * ( 3500 - 1100 ) + 1100 ); // fl_value * ( max - min ) + min
	calls::GetCurrentCamera( )->set_distance( global::camera_distance = value );
}

void ChangerTreeChanged_Handler( ) {
	std::unordered_map<std::string, float> tree_models = std::unordered_map<std::string, float>( {
		{ "models/props_structures/crystal003_refract.vmdl", 1} ,
		{ "models/props_structures/pumpkin001.vmdl", 1.08 },
		{ "models/props_structures/pumpkin003.vmdl", 3 },
		{ "models/props_diretide/pumpkin_head.vmdl", 3 },
		{ "models/props_gameplay/pumpkin_bucket.vmdl", 1 },
		{ "maps/jungle_assets/trees/pitcher/jungle_pitcher_tree.vmdl", 2 },
		{ "models/props_garden/tree_garden001.vmdl", 0.5 },
		{ "maps/journey_assets/props/trees/journey_armandpine/journey_armandpine_02_stump.vmdl", 4.5 },
		{ "models/props_tree/frostivus_tree.vmdl", 0.85 },
		{ "models/props_tree/ti7/ggbranch.vmdl", 1 },
		{ "models/props_tree/newbloom_tree.vmdl", 0.9 },
	} );

	std::unordered_map<std::string, std::string> tree_model_names = std::unordered_map<std::string, std::string>( {
		{ "Crystal", "models/props_structures/crystal003_refract.vmdl"} ,
		{ "Pumpkins #1", "models/props_structures/pumpkin001.vmdl"},
		{ "Pumpkins #2", "models/props_structures/pumpkin003.vmdl"},
		{ "Pumpkins #3","models/props_diretide/pumpkin_head.vmdl"},
		{ "Pumpkin Buckets","models/props_gameplay/pumpkin_bucket.vmdl" },
		{ "Jungle pitcher", "maps/jungle_assets/trees/pitcher/jungle_pitcher_tree.vmdl" },
		{ "Tree garden", "models/props_garden/tree_garden001.vmdl" },
		{ "Stumps", "maps/journey_assets/props/trees/journey_armandpine/journey_armandpine_02_stump.vmdl" },
		{ "Frostivus", "models/props_tree/frostivus_tree.vmdl" },
		{ "TI7 GG Tree", "models/props_tree/ti7/ggbranch.vmdl" },
		{ "New Bloom", "models/props_tree/newbloom_tree.vmdl" },
	} );


	CDOTA_BinaryObjectSystem* binary_obj_sys = (CDOTA_BinaryObjectSystem*)util::find_game_system( "CDOTA_BinaryObjectSystem" );
	CUIPanel* selected = panorama_gui.changer_TreesDropDown->panel2d_as<CDropDown>( )->GetSelected( );

	if ( !selected || !binary_obj_sys )
		return;

	const char* dropdown_sel = reinterpret_cast<CLabel*>( selected )->label_text( );

	if ( !dropdown_sel || (std::uintptr_t)dropdown_sel == 0x1 || !tree_model_names.contains( dropdown_sel ) )
		return;

	some_function scale_changed = CNetworkMessages::get( )->find_network_callback( "gameSceneNodeLocalScaleChanged" );
	for ( C_DOTA_MapTree* ent : binary_obj_sys->m_trees ) {

		if ( !ent )
			continue;

		SchemaVClass* game_scene = ent->schema_member<SchemaVClass*>( "client.dll/C_BaseEntity/m_pGameSceneNode" );

		ent->set_model( tree_model_names[ dropdown_sel ] );

		if ( game_scene ) {
			game_scene->schema_member<float>( "client.dll/CGameSceneNode/m_flScale" ) = tree_models[ tree_model_names[ dropdown_sel ] ];
			game_scene->CallVFunc<10>( 4 );
		}

		if ( scale_changed )
			scale_changed( ent );
	}
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