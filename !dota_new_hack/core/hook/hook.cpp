#include "hook.hpp"
#include "../../gui/gui.hpp"
#include "../../gui/panorama_gui.h"
#include "../features/inventory_changer.h"
#include "../features/hero_bar.h"
#include "../features/camera_hack.h"

extern CGui* pGui;
extern CPanoramaGUI panorama_gui;
extern CGameEntitySystem* g_pGameEntitySystem;
C_DOTAGamerules* g_pGameRules;


void EntityEventListener::OnEntityCreated( C_BaseEntity* rcx ) {
	if ( const auto client_class = rcx->client_class( ); ( client_class && client_class->m_pNetworkName ) ) {
		const auto class_name = std::string_view( client_class->m_pNetworkName );
		if ( class_name == "C_DOTAGamerulesProxy" ) { // class id 1438
			g_pGameRules = rcx->schema_member<C_DOTAGamerules*>( "client.dll/C_DOTAGamerulesProxy/m_pGameRules" );
		}

		if ( class_name.starts_with( "C_DOTA_Unit_Hero" ) || class_name.starts_with( "CDOTA_Unit_Hero" ) )
			context.heroes.insert( static_cast<C_DOTA_BaseNPC_Hero*>( rcx ) );

		context.entities[ rcx->index( ) ] = rcx;
	}
}

void EntityEventListener::OnEntityDeleted( C_BaseEntity* rcx ) {
	if ( const auto client_class = rcx->client_class( ); ( client_class && client_class->m_pNetworkName ) ) {
		const auto class_name = std::string_view( client_class->m_pNetworkName );

		if ( class_name.starts_with( "C_DOTA_Unit_Hero" ) || class_name.starts_with( "CDOTA_Unit_Hero" ) && context.heroes.contains( static_cast<C_DOTA_BaseNPC_Hero*>( rcx ) ) )
			context.heroes.erase( static_cast<C_DOTA_BaseNPC_Hero*>( rcx ) );

		context.entities.erase( rcx->index( ) );
	}
}

void hook::functions::CreateMove( CDOTAInput* rcx, int slot, bool should_fill_weaponselect ) {
	reinterpret_cast<decltype( &CreateMove )>( hook::original::fpCreateMove )( rcx, slot, should_fill_weaponselect );

	auto user_cmd = (CDota2UserCmdPB*)( (char*)rcx + 0x68 * ( rcx->m_sequence_number % 150 ) + slot + 0x10 );

	if ( user_cmd->has_crosshairtrace( ) && user_cmd->has_cameraposition_x( ) && user_cmd->has_cameraposition_y( ) )
		context.traced_cursor = vector3d{ user_cmd->crosshairtrace( ).x( ), user_cmd->crosshairtrace( ).y( ), user_cmd->crosshairtrace( ).z( ) };
	else context.traced_cursor.Zero( );
}

bool hook::functions::BAsyncSendProto( CProtoBufMsgBase* protobufMsg, IProtoBufSendHandler* handler, google::protobuf::Message* responseMsg, unsigned int respMsgID ) {
	//spdlog::debug( "sent message to GC: {}\n", protobufMsg->msgID );

	if ( protobufMsg->msgID == k_EMsgClientToGCEquipItems ) {
		features::inventory_changer.process_equips( (CMsgClientToGCEquipItems*)protobufMsg->msg );
	}

	return reinterpret_cast<decltype( &BAsyncSendProto )>( hook::original::fpBAsyncSendProto )( protobufMsg, handler, responseMsg, respMsgID );
}

EGCResults hook::functions::SGCRetrieveMessage( ISteamGameCoordinator* thisptr, uint32* punMsgType, ProtoBufMsgHeader_t* pubDest, uint32 cubDest, uint32* pcubMsgSize ) {
	const auto ret = reinterpret_cast<decltype( &SGCRetrieveMessage )>( hook::original::fpSGCRetrieveMessage )( thisptr, punMsgType, pubDest, cubDest, pcubMsgSize );
	const auto msg_id = *punMsgType & 0x7FFFFFFF;
	//spdlog::debug( "received message from GC: {}\n", msg_id );

	auto body_data = (void*)( ( (std::uintptr_t)pubDest ) + 8 );
	auto body_size = *pcubMsgSize - 8;
	if ( panorama_gui.log_searching_players && msg_id == 7198 ) { // k_EMsgGCMatchmakingStatsResponse
		CMsgDOTAMatchmakingStatsResponse body;

		if ( body.ParsePartialFromArray( body_data, body_size ) ) {
			constexpr const auto matchgroups = mapbox::eternal::map<std::uint8_t, mapbox::eternal::string>( {
				{ 0, "#dota_matchgroup_uswest" },
				{ 1, "#dota_matchgroup_useast"},
				{ 3, "#dota_matchgroup_sea"},
				{ 2, "#dota_matchgroup_eu" },
				{ 8, "#dota_matchgroup_vie" },
				{ 4, "#dota_matchgroup_cn" },
				{ 7, "#dota_matchgroup_ru" },
				{ 9, "#dota_matchgroup_au" },
				{ 10, "#dota_matchgroup_jnb" },
				{ 13, "#dota_matchgroup_dubai" },
				{ 5, "#dota_matchgroup_brazil" },
				{ 14, "#dota_matchgroup_chile" },
				{ 15, "#dota_matchgroup_peru" },
				{ 6, "#dota_matchgroup_argentina" },
				{ 16, "#dota_matchgroup_india" },
				{ 19, "#dota_matchgroup_japan" },
				{ 11, "#dota_matchgroup_pw_telecom_shanghai" },
				{ 12, "#dota_matchgroup_pw_unicom" },
				{ 17, "#dota_matchgroup_pw_telecom_guangdong" },
				{ 18, "#dota_matchgroup_pw_telecom_zhejiang" },
				{ 20, "#dota_matchgroup_pw_telecom_wuhan" },
				{ 25, "#dota_matchgroup_pw_unicom_tianjin" },
				{ 21, "#dota_matchgroup_taiwan" }
				} );

			for ( auto iterator = 0; iterator < body.match_groups_size( ); ++iterator ) {
				const auto& group = body.match_groups( iterator );

				if ( const auto searching = group.players_searching( ); searching > 0 ) {
					const auto res = CLocalize::GetInstance( ).FindStringSafely( matchgroups.at( iterator ).data( ) );
					std::wcout << util::utf8_decode( res ) << L": " << searching << "\n";
				}
			}
		}
	}
	if ( msg_id == 26 && panorama_gui.auto_accept ) { // k_ESOMsg_UpdateMultiple
		CMsgSOMultipleObjects body;
		if ( body.ParsePartialFromArray( body_data, body_size ) ) {

			if ( body.objects_modified_size( ) == 5 ) {
				const auto inversed_lobby_id = ~( CGCClient::get( ).lobby_manager( )->find_dota_lobby( )->lobby_id( ) );
				const uint32_t account_id = reinterpret_cast<CDOTAPlayerInventory*>( CGCClient::get( ).so_listeners( )[ 1 ] )->m_soid.m_steamid;

				CMsgReadyUp msg;
				msg.set_ready_up_key( inversed_lobby_id ^ ( account_id | ( (uint64_t)account_id << 32 ) ) );
				msg.set_state( DOTALobbyReadyState_ACCEPTED );
				ISteamClient::get( ).GetISteamGenericInterface( ISteamClient::GetHSteamPipe( ), ISteamClient::GetHSteamUser( ), "SteamGameCoordinator001" )
					->send_msg( msg, 7070 ); // k_EMsgGCReadyUp
			}
		}
	}

	return ret;
}

bool hook::functions::SendNetMessage( INetChannel* thisptr, NetMessageHandle_t* rdx, google::protobuf::Message* msg, NetChannelBufType_t type ) {
	bool ret = reinterpret_cast<decltype( &SendNetMessage )>( hook::original::fpSendNetMessage )( thisptr, rdx, msg, type );
	return ret;
}

void* hook::functions::PostReceivedNetMessage( INetChannel* rcx, CNetworkSerializerPB* rdx, google::protobuf::Message* r8, NetChannelBufType_t* r9 ) {
	if ( rdx->messageID == DOTA_UM_TE_DotaBloodImpact ) return 0;
	else if ( rdx->messageID == GE_SosStartSoundEvent ) {
		auto msg_ = (CMsgSosStartSoundEvent*)r8;
		C_BaseEntity* ent;

		if ( ent = g_pGameEntitySystem->find_entity( msg_->source_entity_index( ) ); !ent || !ent->client_class( ) )
			goto end;

		if ( util::fast_strstr( ent->client_class( )->m_pNetworkName, "Creep" ) ) {
			return 0;
		}
	}
	else if ( rdx->messageID == UM_ParticleManager ) {
		CUserMsg_ParticleManager* particle_manager = static_cast<CUserMsg_ParticleManager*>( r8 );

		if ( panorama_gui.draw_particles && particle_manager->type( ) == GAME_PARTICLE_MANAGER_EVENT_SET_FOW_PROPERTIES ) {
			particle_manager->mutable_set_particle_fow_properties( )->set_fow_radius( 999999.f );

		}
	}
	else if ( rdx->messageID == GE_Source1LegacyGameEvent ) {
		CGameEvent* deserialized = CGameEventManager::GetInstance( )->UnserializeEvent( (CMsgSource1LegacyGameEvent*)r8 );
		spdlog::info( "{}\n", deserialized->GetName( ) );
	}
	else if ( rdx->messageID == svc_PacketEntities ) {
		CSVCMsg_PacketEntities* packet_entities = (CSVCMsg_PacketEntities*)r8;
		auto _data = proto_string( packet_entities->entity_data( ) );
		if ( !util::exists( (void*)_data ) ) goto end;
		auto updated_entries = packet_entities->updated_entries( );

		while ( updated_entries-- != 0 ) {

			bitstream bs{ _data };
			while ( bs.good( ) ) {
				C_BaseEntity* entity = context.entities[ bs.readUBitVar( ) ];

				if ( !entity || !entity->identity( ) || !entity->identity( )->entity_name( ) )
					break;

				int32_t etype = 0;
				if ( bs.readBool( ) ) {
					if ( bs.readBool( ) ) etype = 4;
					else etype = 3;
				}
				else {
					if ( bs.readBool( ) ) etype = 1;
					else etype = 2;
				}

				if ( util::fast_strstr( entity->identity( )->entity_name( ), "hero" ) )
					spdlog::debug( "{}: {}, delta: {}\n", entity->identity( )->entity_name( ), etype );
			}
		}
	}

	if ( rdx->messageID != net_Tick )
		goto end;

	if ( global::in_game = IEngineClient::GetInstance( ).in_game( ); global::in_game ) {

		if ( !global::g_LocalEntity ) {
			static C_DOTAPlayerController** players = (decltype( players ))util::get_absolute_address( util::find_pattern( global::client, "\x48\x8B\x05\xCC\xCC\xCC\xCC\x89\xBE", "", false ), 3, 7 );

			if ( const auto local_controller = players[ 0 ]; local_controller ) {
				global::g_LocalEntity = g_pGameEntitySystem->find_entity_by_handle<std::uintptr_t>( local_controller->assigned_hero( ) );

				ICVar::get( )[ "r_farz" ]->m_values.flt = 10000.f;
#ifdef _DEBUG
				ICVar::get( )[ "stats_display" ]->m_values.i32 = 5;
#endif
			}
		}

		static bool camera_hooked = false;
		if ( !camera_hooked && calls::GetCurrentCamera ) {
			if ( auto camera = reinterpret_cast<VClass*>( calls::GetCurrentCamera( ) ); camera ) {
				hook::install_hook( camera->GetVF( 9 ), &hook::functions::OnMouseWheeled, &hook::original::fpOnMouseWheeled, "C_DOTACamera::OnMouseWheeled" );
				camera_hooked = true;
			}
		}

		if ( g_pGameRules->game_state( ) == DOTA_GAMERULES_STATE_GAME_IN_PROGRESS || g_pGameRules->game_state( ) == DOTA_GAMERULES_STATE_PRE_GAME ) {

			if ( panorama_gui.draw_networthdelta ) {
				util::set_timer( []( ) {
					CUIPanel* DotaHud = CPanoramaUIEngine::GetInstance( )->engine_source2( )->find_panel( "DotaHud" );
					CPanel2D* topbar = DotaHud->find_child_traverse( "topbar" )->panel2d_as( );
					C_DOTA_PlayerResource* resource = C_DOTA_PlayerResource::get( );

					int goodguys_top = 0;
					int badguys_top = 0;
					for ( int i = 0; i != 64; ++i ) {
						const int TeamNum = resource->GetTeam( i );
						if ( TeamNum == 2 ) // radiant
							goodguys_top += resource->GetNetWorthOfPlayer( i );
						else if ( TeamNum == 3 ) // dire
							badguys_top += resource->GetNetWorthOfPlayer( i );
					}
					calls::CDOTA_Hud_Top_Bar__UpdateNetWorthDifference( topbar, goodguys_top, badguys_top );
				}, 4000 );
			}
		}
	}
	else {
		if ( !global::g_LocalEntity )
			goto end;

		features::camera_hack.change_distance( );
		global::g_LocalEntity = 0;
		g_pGameRules = nullptr;
		context.heroes.clear( );
	}

end:
	return reinterpret_cast<decltype( &PostReceivedNetMessage )>( hook::original::fpPostReceivedNetMessage )( rcx, rdx, r8, r9 );
}

long hook::functions::Present( IDXGISwapChain* pSwapchain, UINT SyncInterval, UINT Flags ) {
	static bool imgui_inited = false;
	if ( !imgui_inited ) {
		if ( pGui->init( pSwapchain ) ) imgui_inited = true;
	}

	ImGui_ImplDX11_NewFrame( );
	ImGui_ImplWin32_NewFrame( );
	ImGui::NewFrame( );

	// OLD IMGUI MENU
	pGui->Render( );

	if ( global::in_game && g_pGameRules && ( g_pGameRules->game_state( ) == DOTA_GAMERULES_STATE_GAME_IN_PROGRESS || g_pGameRules->game_state( ) == DOTA_GAMERULES_STATE_PRE_GAME ) ) {

		if ( panorama_gui.draw_health )
			features::hero_bar.draw_health( panorama_gui.draw_mana_bar );


		C_DOTA_BaseNPC_Hero* closest_hero = nullptr;
		double lowest_distance = DBL_MAX;

		for ( C_DOTA_BaseNPC_Hero* hero : context.heroes ) {

			if ( hero->health( ) <= 0 && hero->identity( )->dormant( ) && hero->illusion( ) )
				continue;

			vector2d scr;
			vector3d pos = hero->abs_origin( );
			pos.z += hero->health_bar_offset( );

			if ( CRenderGameSystem::GetInstance( )->GetVectorInScreenSpace( pos, scr ) ) {
				std::uint16_t cc = 0;

				for ( auto modifier : hero->modifier_manager( )->GetModifiers( ) ) {
					cc += 25;
					ImGui::GetForegroundDrawList( )->AddText( ImVec2{ scr.x - 30, scr.y + cc }, 0xFFFFFFFF, modifier->GetBuffName( ) );
				}
			}

			if ( const double distance = hero->abs_origin( ).dist_to( context.traced_cursor ); distance < lowest_distance ) {
				lowest_distance = distance;
				closest_hero = hero;
			}
		}

		if ( closest_hero && lowest_distance < 1000.f ) {
			vector2d scr;
			vector3d pos = closest_hero->abs_origin( );
			pos.z += closest_hero->health_bar_offset( );

			if ( CRenderGameSystem::GetInstance( )->GetVectorInScreenSpace( pos, scr ) ) {
				std::uint16_t cc = 0;
				char buf[ 256 ];
				sprintf_s( buf, "closest: %s, dist: %2.f", closest_hero->identity( )->entity_name( ), lowest_distance );
				ImGui::GetForegroundDrawList( )->AddText( ImVec2{ scr.x, scr.y }, 0xFFFFFFFF, buf );
			}

		}
	}

	ImGui::Render( );

	pGui->pContext->OMSetRenderTargets( 1, &pGui->pRenderTargetView, nullptr );
	ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );
	return reinterpret_cast<decltype( &Present )>( hook::original::fpPresent )( pSwapchain, SyncInterval, Flags );
}

void hook::functions::OnMouseWheeled( CDOTA_Camera* rcx, int delta ) {
	reinterpret_cast<decltype( &OnMouseWheeled )>( hook::original::fpOnMouseWheeled )( rcx, delta );

	if ( pGui->mouse_distance && !pGui->show && global::in_game ) {
		features::camera_hack.on_mouse_wheeled( rcx, delta );
		if ( panorama_gui.camera_dist_slider ) {
			const auto casted2volvotype = static_cast<float>( features::camera_hack.get_distance( ) - features::camera_hack.get_min_distance( ) ) / ( features::camera_hack.get_max_distance( ) - features::camera_hack.get_min_distance( ) );
			panorama_gui.camera_dist_slider->children( )[ 1 ]->panel2d_as<CSlider>( )->set_fl( casted2volvotype );
		}
	}
}

LRESULT __stdcall hook::functions::WndProc( const HWND hWnd, const unsigned int uMsg, const uintptr_t wParam, const uintptr_t lParam ) {
	if ( uMsg == WM_KEYUP ) {
		if ( wParam == VK_F1 ) {
			//auto bg = CPanoramaUIEngine::GetInstance( )->engine_source2( )->find_panel( "DotaDashboard" )->find_child_traverse( "DashboardBackgroundManager" )->children( )[ 0 ];
			//bg->set_style( "background-image: url(\"file://{resources}/ambg.vtex\");" );

			auto radiant_players = CPanoramaUIEngine::GetInstance( )->engine_source2( )->find_panel( "DotaHud" )->find_child_traverse( "TopBarRadiantPlayersContainer" );
			auto dire_players = CPanoramaUIEngine::GetInstance( )->engine_source2( )->find_panel( "DotaHud" )->find_child_traverse( "TopBarDirePlayersContainer" );

			for ( auto img : radiant_players->find_children_traverse( "HeroImage" ) ) {

				const auto src = img->panel2d_as<CDOTA_UI_HeroImage>( )->image_src( );
				if ( !src ) continue;

				std::string src2 = { std::string_view{src}.substr( 23 ).data( ) };
				const auto formatted_name = src2.erase( src2.size( ) - 4 );

				spdlog::info( "Radiant player: {}\n", formatted_name );
			}
			for ( auto img : dire_players->find_children_traverse( "HeroImage" ) ) {

				const auto src = img->panel2d_as<CDOTA_UI_HeroImage>( )->image_src( );
				if ( !src ) continue;

				std::string src2 = { std::string_view{src}.substr( 23 ).data( ) };
				const auto formatted_name = src2.erase( src2.size( ) - 4 );

				spdlog::info( "Dire player: {}\n", formatted_name );
			}
		}
		if ( wParam == VK_F2 ) {
			int idx;
			if ( CVar* ptr = ICVar::get( ).register_convar( "dota2", &idx ); ptr ) {
				spdlog::debug( "Registered cvar: {}, ptr: {}\n", idx, (void*)ptr->m_name );
			}
		}
		if ( wParam == VK_F3 ) {
			panorama_gui.show( );
		}
		if ( wParam == VK_F4 ) {
		}
		if ( wParam == VK_INSERT ) {
			CPanoramaUIEngine::GetInstance( )->engine_source2( )->play_sound_effect( "ui_menu_activate_open" );
			pGui->show ^= true;
		}
		if ( wParam == VK_HOME ) {

			CDOTA_ParticleManager::GetInstance( )->destroy_own_particles( );

			constexpr auto unhook = []( void* ) -> unsigned long {
				features::camera_hack.change_distance( );
				global::g_LocalEntity = 0;
				global::g_mapItemIcons.clear( );
				global::g_mapSpellIcons.clear( );

				hook::uninstall_all_hooks( );
				ImGui_ImplDX11_Shutdown( );
				ImGui_ImplWin32_Shutdown( );
				ImGui::DestroyContext( );

				SetWindowLongPtrW( pGui->retWND( ), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>( hook::original::fpWndProc ) );
				FreeConsole( );
				FreeLibraryAndExitThread( global::hModule, 0 );
			};
			CreateThread( nullptr, 0, unhook, nullptr, 0, 0 );
			return 1;
		}
	}

	if ( true && ImGui_ImplWin32_WndProcHandler( hWnd, uMsg, wParam, lParam ) ) return 1;

	return CallWindowProcA( (WNDPROC)hook::original::fpWndProc, hWnd, uMsg, wParam, lParam );
}



bool hook::install_hook( std::uintptr_t address, LPVOID function, LPVOID* original, std::string_view name ) {
	static bool init = false;
	if ( !init ) {
		MH_Initialize( );
		init = true;
	}

	if ( MH_CreateHook( reinterpret_cast<LPVOID>( address ), function, original ) == MH_ERROR_ALREADY_CREATED ) {
		spdlog::warn( "[hook::install_hook]: Hook {} is already created\n", name );
	}
	const auto status = MH_EnableHook( reinterpret_cast<LPVOID>( address ) );
	if ( status == MH_STATUS::MH_OK ) {
		hook::hooks[ address ] = name;
	}
	else {
		spdlog::error( "[hook::install_hook]: Hook {} is not installed\n", name );
	}
	return status == MH_STATUS::MH_OK;
}

bool hook::uninstall_hook( std::uintptr_t address ) {
	const auto status = MH_DisableHook( reinterpret_cast<LPVOID>( address ) ) == MH_STATUS::MH_OK;

	if ( status && hook::hooks.contains( address ) ) {
		const auto& name = hook::hooks.at( address );
		hook::hooks.erase( address );
	}

	return status;
}

void hook::uninstall_all_hooks( ) {
	for ( const auto& addr : hooks ) {
		hook::uninstall_hook( addr.first );
		MH_RemoveHook( reinterpret_cast<LPVOID>( addr.first ) );
	}
}