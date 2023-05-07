#include "hook.hpp"

#include "../../gui/gui.hpp"

#include "../sdk_game/CDOTA_InventoryManager.hpp"
#include "../lua/events.hpp"

extern CGui* pGui;
extern CGameEntitySystem* g_pGameEntitySystem;
extern CDOTAInventoryManager* g_pInventoryManager;
extern CDOTAGCClientSystem* g_pGCClientSystem;

extern CheatData cheat_data;

bool hook::install_hook( std::uintptr_t address, LPVOID function, LPVOID* original, std::string_view name ) {
	static bool init = false;
	if ( !init ) {
		MH_Initialize( );
		init = true;
	}

	MH_CreateHook( reinterpret_cast<LPVOID>( address ), function, original );
	const auto status = MH_EnableHook( reinterpret_cast<LPVOID>( address ) );
	if ( status == MH_STATUS::MH_OK ) {
		hook::hooks[name] = address;

#ifdef _DEBUG
		util::set_console_color( 0x1 );
		std::cout << name;
		util::set_console_color( );
		std::cout << " hooked at "; util::set_console_color( 0xA ); std::cout << "0x" << std::uppercase << std::hex << address << std::endl;
		util::set_console_color( );
#endif
	}
	return status == MH_STATUS::MH_OK;
}

void hook::uninstall_hook( ) {
	for ( const auto& addr : hooks ) {
		MH_DisableHook( reinterpret_cast<LPVOID>( addr.second ) );
	}
}

constexpr auto proto_size = sizeof( ProtoBufMsgHeader_t );

void hook::functions::RunScript( CUIEngine* thisptr, CPanel2D* panel, const char* entireJSCode, const char* pathToXMLContext, int int1 /*unk bool mb*/, int int2/*rnd val*/, bool alreadyCompiled )
{
	hook::original::fpRunScript( thisptr, panel, entireJSCode, pathToXMLContext, int1, int2, alreadyCompiled );
}

bool hook::functions::BAsyncSendProto( CProtoBufMsgBase* protobufMsg, IProtoBufSendHandler* handler, google::protobuf::Message* responseMsg, unsigned int respMsgID ) {
	std::cout << "BAsyncSendProto: " << protobufMsg->msgID << " | " << EDOTAGCMsg2String( protobufMsg->msgID ) << std::endl;

	if ( protobufMsg->msgID == k_EMsgClientToGCEquipItems ) {
		auto inventory = g_pInventoryManager->GetCDOTAPlayerInventory( );
		auto msg = reinterpret_cast<CMsgClientToGCEquipItems*>( protobufMsg->msg );
		auto& equip = msg->equips( ).Get( 0 );
		CEconItem* item = inventory->FindItem( equip.item_id( ) );
		if ( equip.has_new_slot( ) ) {
			std::cout << std::format( "Equipping {}. Class: {}; Slot: {}\n",
									  (void*)item,
									  equip.new_class( ),
									  equip.new_slot( )
			);
			item->Class( ) = equip.new_class( );
			item->Slot( ) = equip.new_slot( );
			item->Flag( ) = 0x200 | 0x100;

			const auto x = inventory->SOUpdated( inventory->m_pSOID, item, ESOCacheEvent::eSOCacheEvent_Incremental );
			std::cout << x << std::endl;
			return false;
		}
	}

	return hook::original::fpBAsyncSendProto( protobufMsg, handler, responseMsg, respMsgID );
}

bool hook::functions::DispatchPacket( CGCClient* thisptr, IMsgNetPacket* netPacket )
{
	return hook::original::fpDispatchPacket( thisptr, netPacket );
}

EGCResults hook::functions::ISteamGameCoordinator__SendMessage( ISteamGameCoordinator* thisptr, uint32 unMsgType, ProtoBufMsgHeader_t* pubData, uint32 cubData ) {
	auto ret = hook::original::fpSClientSendMessage( thisptr, unMsgType, pubData, cubData );
	EDOTAGCMsg msg_id = (EDOTAGCMsg)( unMsgType & 0x7FFFFFFF );
	std::cout << "SendMessage: " << msg_id << " | " << EDOTAGCMsg2String( msg_id ) << std::endl;

	return ret;
}

EGCResults hook::functions::ISteamGameCoordinator__RetrieveMessage( ISteamGameCoordinator* thisptr, uint32* punMsgType, ProtoBufMsgHeader_t* pubDest, uint32 cubDest, uint32* pcubMsgSize ) {
	auto ret = hook::original::fpSClientRetrieveMessage( thisptr, punMsgType, pubDest, cubDest, pcubMsgSize );
	EDOTAGCMsg msg_id = (EDOTAGCMsg)( *punMsgType & 0x7FFFFFFF );
	std::cout << "RetrieveMessage: " << msg_id << " | " << EDOTAGCMsg2String( msg_id ) << std::endl;

	return ret;
}

bool hook::functions::ISteamGameCoordinator__IsMessageAvailable( ISteamGameCoordinator* thisptr, uint32* pcubMsgSize ) {
	return hook::original::fpSClientIsMessageAvailable( thisptr, pcubMsgSize );
}

CEntityInstance* hook::functions::OnAddEntity( CGameEntitySystem* rcx, C_BaseEntity* rdx, unsigned int r8 ) {
	CEntityInstance* ret = hook::original::fpOnAddEntity( rcx, rdx, r8 );
	const auto binding = rdx->schema_dynamic_binding( )->m_binary_name;

	if ( binding ) {
		if ( util::fast_strstr( binding, "DOTA_Unit" ) || util::fast_strstr( binding, "DOTA_Base" ) ) {

			cheat_data.UnitList.insert( static_cast<C_DOTA_BaseNPC*>( rdx ) );
			if ( util::fast_strstr( binding, "DOTA_Unit_Hero" ) ) {
				cheat_data.m_hero_list.push_back( static_cast<C_DOTA_BaseNPC_Hero*>( rdx ) );

				auto& UIEngine = CPanoramaUIEngine::GetInstance( );
				CUIPanel* hero_panel = UIEngine.m_pUIEngineSource2->CreatePanel( UIEngine.m_pUIEngineSource2->MakeSymbol( "Panel" ), "HeroBar", UIEngine.find_panel( "DotaHud" ) )->ui_panel( );
				if ( UIEngine.m_pUIEngineSource2->IsValidPanelPointer( hero_panel ) ) {
					hero_panel->load_layout( R"xml(file://{resources}/bar.xml)xml", true );
					cheat_data.m_hero_panels[static_cast<C_DOTA_BaseNPC_Hero*>( rdx )] = hero_panel;
				}
			}
		}
	}

	return ret;
}

CEntityInstance* hook::functions::OnRemoveEntity( CGameEntitySystem* rcx, C_BaseEntity* rdx, unsigned int r8 ) {
	CEntityInstance* ret = hook::original::fpOnRemoveEntity( rcx, rdx, r8 );
	const auto binding = rdx->schema_dynamic_binding( )->m_binary_name;

	if ( binding ) {
		if ( util::fast_strstr( binding, "DOTA_Unit" ) || util::fast_strstr( binding, "DOTA_Base" ) ) {

			cheat_data.UnitList.erase( static_cast<C_DOTA_BaseNPC*>( rdx ) );
			if ( util::fast_strstr( binding, "DOTA_Unit_Hero" ) ) {
				for ( unsigned short j = 0; j < cheat_data.m_hero_list.size( ); j++ ) {
					if ( cheat_data.m_hero_list[j] == rdx )
						cheat_data.m_hero_list.erase( cheat_data.m_hero_list.begin( ) + j );
				}
				const auto casted = static_cast<C_DOTA_BaseNPC_Hero*>( rdx );
				if ( cheat_data.m_hero_panels.contains( casted ) ) {
					cheat_data.m_hero_panels[casted]->delete_async( );
					cheat_data.m_hero_panels.erase( casted );
				}
			}
		}
	}

	return ret;
}

void __fastcall hook::functions::PrepareUnitOrders( C_DOTAPlayerController* player, DotaUnitOrder_t order, int targetidx, vector3d* move_pos, int abilityidx, PlayerOrderIssuer_t orderIssuer, C_DOTA_BaseNPC* entity, bool queue, bool showEffect ) {
	hook::original::fpPrepareUnitOrders( player, order, targetidx, move_pos, abilityidx, orderIssuer, entity, queue, showEffect );
}

INetChannel* hook::functions::CreateNetChannel( CNetworkSystem* rcx, int rdx, void* r8, const char* r9, unsigned int r10, unsigned int r11 ) {
	cheat_data.m_pNetChannel = hook::original::fpCreateNetChannel( rcx, rdx, r8, r9, r10, r11 );

	const auto aPostReceivedNetMessage = util::vmt( reinterpret_cast<uintptr_t>( cheat_data.m_pNetChannel ), 86 );
	hook::install_hook( aPostReceivedNetMessage, &hook::functions::PostReceivedNetMessage, reinterpret_cast<LPVOID*>( &hook::original::fpPostReceivedNetMessage ), "PostReceivedNetMessage" );

	const auto aSendNetMessage = util::vmt( reinterpret_cast<uintptr_t>( cheat_data.m_pNetChannel ), 69 );
	hook::install_hook( aSendNetMessage, &hook::functions::SendNetMessage, reinterpret_cast<LPVOID*>( &hook::original::fpSendNetMessage ), "SendNetMessage" );

	return cheat_data.m_pNetChannel;
}

bool __fastcall hook::functions::SendNetMessage( INetChannel* rcx, NetMessageHandle_t* rdx, google::protobuf::Message* r8, NetChannelBufType_t r9 ) {
	if ( global::inited ) {
		if ( rdx->messageID != 4 && rdx->messageID != 55 && rdx->messageID != 21 && rdx->messageID != 45 && rdx->messageID != 154 ) {
			//char b[256]{ '\0' };
			//std::cout << rdx->protobufBinding->ToString( r8, &b );
		}
	}

	return hook::original::fpSendNetMessage( rcx, rdx, r8, r9 );
}

void* __fastcall hook::functions::PostReceivedNetMessage( INetChannel* rcx, CNetworkSerializerPB* rdx, google::protobuf::Message* r8, NetChannelBufType_t* r9 ) {
	if ( global::inited ) {
		if ( rdx->messageID != 4 && rdx->messageID != 55 && rdx->messageID != 21 && rdx->messageID != 45 && rdx->messageID != 154 ) {
			//char b[256]{ '\0' };
			//std::cout << rdx->protobufBinding->ToString( r8, &b );

		}

		if ( pGui->partial_maphack ) {

			if ( rdx->messageID == 471 /*CDOTAUserMsg_CreateLinearProjectile*/ ) {
				CDOTAUserMsg_CreateLinearProjectile* msg = static_cast<CDOTAUserMsg_CreateLinearProjectile*>( r8 );

				if ( msg->has_entindex( ) && msg->has_origin( ) ) {
					const auto ent_idx = msg->entindex( );

					if ( C_BaseEntity* entity = g_pGameEntitySystem->find_entity( ent_idx ); entity->identity( )->dormant( ) ) {
						ProjectileData_t obj;
						const auto& origin = msg->origin( );

						obj.attached_entity = entity;
						obj.entity_idx = msg->entindex( );
						obj.position = { origin.x( ), origin.y( ), origin.z( ) };
						obj.projectile_handle = msg->handle( );
						obj.create_time = global::g_flGameTime;

						cheat_data.PROJECTILE_LIST[obj.projectile_handle] = obj;
					}
				}
			}

			if ( rdx->messageID == 145 /*CUserMsg_ParticleManager*/ ) {
				CUserMsg_ParticleManager* msg = static_cast<CUserMsg_ParticleManager*>( r8 );
				const auto type = msg->type( );

				if ( type == PARTICLE_MESSAGE::GAME_PARTICLE_MANAGER_EVENT_CREATE ) {

					const auto& particle = msg->create_particle( );
					const auto particle_idx = msg->index( );
					const auto ent_handle = particle.entity_handle( );

					C_BaseEntity* entity = g_pGameEntitySystem->find_entity( ent_handle & 0x3FFF );
					ParticleData_t obj;

					obj.particle_idx = msg->index( );
					if ( util::does_exists( entity ) ) {
						obj.attached_entity = entity;
					}
					obj.name_idx = particle.particle_name_index( );
					obj.create_time = global::g_flGameTime;
					obj.resource_name = IResourceSystem::GetInstance( ).GetResourceByHash<"vpcf">( particle.particle_name_index( ) );
					std::cout << obj.resource_name << std::endl;

					if ( !obj.attached_entity && util::fast_strstr( obj.resource_name.c_str( ), "particles/units/heroes/hero" ) ) {
						static std::smatch matched;
						const std::regex re( R"(particles/units/heroes/(.+)/(.+))" );
						std::string hero_name{ "npc_dota_" };

						if ( std::regex_search( obj.resource_name, matched, re ) ) {
							hero_name.append( matched[1].str( ) );
						}

						for ( auto entx : cheat_data.UnitList ) {
							if ( entx->identity( )->GetName( ) == hero_name ) {
								obj.attached_entity = entx;
							}
						}
					}
					if ( obj.resource_name == "particles/neutral_fx/roshan_spawn.vpcf" ) {
						cheat_data.m_flRoshanSpawned = obj.create_time;
					}
					cheat_data.PARTICLE_LIST[particle_idx] = obj;
				}

				else if ( type == PARTICLE_MESSAGE::GAME_PARTICLE_MANAGER_EVENT_UPDATE_ENT ) {

					const auto& particle = msg->update_particle_ent( );
					const auto particle_idx = msg->index( );
					const auto ent_handle = particle.entity_handle( );

					if ( cheat_data.PARTICLE_LIST.count( particle_idx ) ) {

						ParticleData_t obj = cheat_data.PARTICLE_LIST[particle_idx];

						const auto& pos = particle.fallback_position( );
						obj.position = { pos.x( ), pos.y( ), pos.z( ) };
						if ( !obj.attached_entity )
							obj.attached_entity = g_pGameEntitySystem->find_entity( ent_handle & 0x3FFF );

						cheat_data.PARTICLE_LIST[particle_idx] = obj;
					}
				}

				else if ( type == PARTICLE_MESSAGE::GAME_PARTICLE_MANAGER_EVENT_UPDATE ) {

					const auto& particle = msg->update_particle( );
					const auto particle_idx = msg->index( );

					if ( cheat_data.PARTICLE_LIST.count( particle_idx ) && particle.control_point( ) == 0 ) {

						ParticleData_t obj = cheat_data.PARTICLE_LIST[particle_idx];

						const auto& pos = particle.position( );
						obj.position = { pos.x( ), pos.y( ), pos.z( ) };

						cheat_data.PARTICLE_LIST[particle_idx] = obj;
					}
				}
			}
		}
	}
	return hook::original::fpPostReceivedNetMessage( rcx, rdx, r8, r9 );
}

void __fastcall hook::functions::RunFrame( uintptr_t rcx ) {
	std::once_flag find_hud_size;
	static bool statusBarPatch = !pGui->ColoredBarCreeps;
	global::g_LocalController = functions_call::GetPlayer( -1 );
	if ( global::g_LocalController ) {
		auto game_rules = C_DOTAGamerules::GetInstance( );
		global::g_iGameMode = game_rules->GameMode( );
		global::g_iGameState = game_rules->GameState( );
		global::g_flGameTime = game_rules->GameTime( );

		for ( unsigned char hero = 0; hero < cheat_data.m_hero_list.size( );) {
			if ( cheat_data.m_hero_list[hero]->schema_dynamic_binding( ) == nullptr ) {
				std::cout << "[LOG] delete" << std::endl;
				cheat_data.m_hero_list.erase( cheat_data.m_hero_list.begin( ) + hero );
			}
			else {
				hero++;
			}
		}

		if ( pGui->HackBars && pGui->ColoredBarCreeps && !statusBarPatch ) {
			VirtualProtect( reinterpret_cast<void*>( global::patterns::ColoredBarCreeps ), 0x3, PAGE_EXECUTE_READWRITE, 0 );
			util::memset( reinterpret_cast<void*>( global::patterns::ColoredBarCreeps ), 0x90, 0x3 );
			statusBarPatch = true;
		}
		else if ( !pGui->ColoredBarCreeps && statusBarPatch ) {
			std::vector<unsigned char> new_bytes = { 0xFF, 0x52, 0x20 };
			VirtualProtect( reinterpret_cast<void*>( global::patterns::ColoredBarCreeps ), new_bytes.size( ), PAGE_EXECUTE_READWRITE, 0 );
			util::memcpy( reinterpret_cast<void*>( global::patterns::ColoredBarCreeps ), new_bytes.data( ), new_bytes.size( ) );
			statusBarPatch = false;
		}
	}
	hook::logics::ReceivingInformation( );
	CLuaEvents::lua_on_update( );
	if ( !cheat_data.m_bCameraHooked ) {
		auto camera = functions_call::GetCurrentCamera( );
		if ( camera ) {

			hook::install_hook( util::vmt( (uintptr_t)camera, 9 ), &hook::functions::OnMouseWheeled, reinterpret_cast<LPVOID*>( &hook::original::fpOnMouseWheeled ), "OnMouseWheeled" );
			hook::install_hook( util::vmt( (uintptr_t)camera, 18 ), &hook::functions::GetFogEnd, reinterpret_cast<LPVOID*>( &hook::original::fpGetFogEnd ), "GetFogEnd" );
			hook::install_hook( util::vmt( (uintptr_t)camera, 19 ), &hook::functions::GetZFar, reinterpret_cast<LPVOID*>( &hook::original::fpGetZFar ), "GetZFar" );
			cheat_data.m_bCameraHooked = true;
		}
	}

	std::call_once( find_hud_size, []( ) {
		cheat_data.dotahud_actuallayoutheight = CPanoramaUIEngine::GetInstance( ).find_panel( "Hud" )->actuallayoutheight( );
		cheat_data.dotahud_actuallayoutwidth = CPanoramaUIEngine::GetInstance( ).find_panel( "Hud" )->actuallayoutwidth( );
					} );

	for ( const auto& hero_data : cheat_data.m_hero_panels ) {
		auto hero = hero_data.first;
		auto panel = hero_data.second;

		if ( !hero->ally( ) && !hero->identity( )->dormant( ) && hero->health( ) >= 1 ) {
			auto abs_origin = hero->abs_origin( ); abs_origin.z += hero->health_bar_offset( );
			vector2d screen_abs_origin;
			if ( functions_helper::w2s( abs_origin, &screen_abs_origin ) ) {

				const auto pos_x = screen_abs_origin.x / cheat_data.dotahud_actuallayoutwidth * 100;
				const auto pos_y = screen_abs_origin.y / cheat_data.dotahud_actuallayoutheight * 100;
				const auto position_style = std::format( "position: {}% {}% 0; visibility: visible;", pos_x, pos_y );
				panel->set_style( position_style );

				const auto mana = hero->mana( );
				const auto mana_ratio = std::floor( mana / hero->max_mana( ) * 100 );
				auto ability = g_pGameEntitySystem->find_entity<C_DOTABaseAbility>( hero->GetAbility( 0 ) );

				auto children = panel->children( );
				children[0]->set_style( std::format( "width: {}%;", mana_ratio ) );
				children[1]->panel2d_as<CLabel*>( )->set_label_text( std::format( "{}%", mana_ratio ).c_str( ) );
				children[2]->panel2d_as<CLabel*>( )->set_label_text( std::to_string( static_cast<int>( mana ) ).c_str( ) );
				children[3]->panel2d_as<CDOTA_UI_AbilityImage*>( )->set_ability_name( ability->identity( )->GetName( ) );

				continue; // чтобы не делать 2 else(для изалли и ворлд2скр)
			}
		}
		panel->set_style( "visibility: collapse;" );
	}


	hook::original::fpRunFrame( rcx );
}

bool hook::functions::FireEventClientSide( CGameEventManager* thisptr, CGameEvent* ev ) {
	if ( global::inited ) {
		if ( const std::string_view eventName = ev->GetName( ); !eventName.empty( ) && eventName != "dota_action_success" ) {
			if ( eventName == "map_shutdown" ) {
				cheat_data.m_bCameraHooked = false;
				cheat_data.m_flRoshanSpawned = -1;
				global::camera_distance = 1200;
				global::g_pDOTAPlayerResource = 0;
				global::g_LocalEntity = 0;
				global::g_LocalController = 0;
				cheat_data.UnitList.clear( );
				cheat_data.m_hero_list.clear( );
				cheat_data.PARTICLE_LIST.clear( );
				cheat_data.PROJECTILE_LIST.clear( );
				cheat_data.ABILITYESP_LIST.clear( );
				for ( const auto& p : cheat_data.m_hero_panels ) {
					p.second->delete_async( );
				}
				cheat_data.m_hero_panels.clear( );
			}
		}
	}
	return hook::original::fpFireEventClientSide( thisptr, ev );
}

long __fastcall hook::functions::Present( IDXGISwapChain* pSwapchain, UINT SyncInterval, UINT Flags ) {
	static bool imgui_inited = false;
	if ( !imgui_inited ) {
		while ( !pGui->init( pSwapchain ) ) return hook::original::fpPresent( pSwapchain, SyncInterval, Flags );
		imgui_inited = true;
	}

	ImGui_ImplDX11_NewFrame( );
	ImGui_ImplWin32_NewFrame( );
	ImGui::NewFrame( );

	CLuaEvents::lua_on_present( );
	pGui->Render( );

	hook::logics::RenderInfo( );

	ImGui::Render( );

	pGui->pContext->OMSetRenderTargets( 1, &pGui->pRenderTargetView, nullptr );
	ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );
	return hook::original::fpPresent( pSwapchain, SyncInterval, Flags );
}

float __fastcall hook::functions::GetFogEnd( CDOTA_Camera* thisptr, float val ) {
	if ( pGui->fog ) {
		return 0.f;
	}
	return hook::original::fpGetFogEnd( thisptr, val );
}

float __fastcall hook::functions::GetZFar( CDOTA_Camera* thisptr, float val ) {
	return global::camera_distance * 2.204f;
}

void __fastcall hook::functions::OnMouseWheeled( CDOTA_Camera* thisptr, int delta ) {
	hook::original::fpOnMouseWheeled( thisptr, delta );

	if ( pGui->mouse_distance && !pGui->show && global::g_iGameMode > EGameMode::INVALID && global::g_iGameState == EGameState::GAME_IN_PROGRESS || global::g_iGameState == EGameState::PRE_GAME && thisptr ) {
		if ( delta == -1 ) {
			global::camera_distance += pGui->camera_step;
		}
		else if ( delta == 1 ) {
			global::camera_distance -= pGui->camera_step;
		}
		if ( global::camera_distance > 3500 ) global::camera_distance = 3500;
		if ( global::camera_distance < 1000 ) global::camera_distance = 1000;

		thisptr->SetDistance( static_cast<float>( global::camera_distance ) );
	}
}

LRESULT __stdcall hook::functions::WndProc( const HWND hWnd, const unsigned int uMsg, const uintptr_t wParam, const uintptr_t lParam ) {
	if ( uMsg == WM_KEYUP ) {
		if ( wParam == VK_F1 ) {
			for ( const auto& pan : cheat_data.m_hero_panels ) {
				//pan.second.r
			}
		}
		if ( wParam == VK_F2 ) {
			auto& UIEngine = CPanoramaUIEngine::GetInstance( );

			CUIPanel* dashboard = UIEngine.find_panel( "DashboardForeground" );
			auto hero_images = dashboard->find_children_traverse( "HeroImage" );
			for ( int i = 0; i < hero_images.size( ); ++i ) {
				CDOTA_UI_Image* hero = hero_images.at( i )->panel2d_as<CDOTA_UI_Image*>( );

				if ( hero->source_text( ).find( "npc_dota_hero_nevermore" ) != -1 ) {
					if ( CUIPanel* suggested_ban = hero->ui_panel( )->parent( )->FindChild( "SuggestedBanOverlay" ); suggested_ban ) {
						suggested_ban->panel_style( )->SetVisibility( true );
					}
				}
			}
		}
		if ( wParam == VK_INSERT ) {
			auto& UIEngineSource2 = CPanoramaUIEngine::GetInstance( ).m_pUIEngineSource2;

			for ( const auto& current : UIEngineSource2->GetPanelList( )->slots ) {
				if ( current.panel && UIEngineSource2->IsValidPanelPointer( current.panel ) && !strcmp( current.panel->name_id( ), "Hud" ) ) {
					UIEngineSource2->RunScript( current.panel,
												"$.DispatchEvent( 'PlaySoundEffect', 'ui_menu_activate_open' );",
												IEngineClient::GetInstance( ).IsInGame( ) ? "panorama/layout/base_hud.xml" : "panorama/layout/base.xml", 0, 0, false );
				}
			}

			pGui->show ^= true;
		}
		if ( wParam == VK_END ) {
			const auto local_inventory = g_pInventoryManager->GetCDOTAPlayerInventory( );
			const uint32_t AccountID = local_inventory->m_pSOID.m_unSteamID & 0xFFFFFFFF;

			CEconItem* item = local_inventory->CreateItemObject( );
			item->m_unDefIndex = 4007; // dc hook
			item->m_ulID = rand( ) % item->m_unDefIndex;
			local_inventory->m_pSharedObjectCache->AddObjectToCache( item );
			local_inventory->SOCreated( local_inventory->m_pSOID, item, ESOCacheEvent::eSOCacheEvent_Incremental );
		}
		if ( wParam == VK_HOME ) {

			CDOTA_ParticleManager::GetInstance( ).DestroyAllParticles( );

			constexpr auto unhook = []( void* ) -> unsigned long {
				cheat_data.m_flRoshanSpawned = -1;
				global::camera_distance = 1200;
				global::g_pDOTAPlayerResource = 0;
				global::g_LocalEntity = 0;
				global::g_LocalController = 0;
				cheat_data.m_hero_list.clear( );
				cheat_data.PARTICLE_LIST.clear( );
				cheat_data.PROJECTILE_LIST.clear( );
				global::g_mapItemIcons.clear( );
				global::g_mapSpellIcons.clear( );
				for ( const auto& p : cheat_data.m_hero_panels ) {
					p.second->delete_async( );
				}
				cheat_data.m_hero_panels.clear( );

				const std::vector<unsigned char> newByte = { 0xFF, 0x52, 0x20 };
				VirtualProtect( reinterpret_cast<void*>( global::patterns::ColoredBarCreeps ), newByte.size( ), PAGE_EXECUTE_READWRITE, 0 );
				util::memcpy( reinterpret_cast<void*>( global::patterns::ColoredBarCreeps ), newByte.data( ), newByte.size( ) );

				hook::uninstall_hook( );
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
	if ( true && ImGui_ImplWin32_WndProcHandler( hWnd, uMsg, wParam, lParam ) ) {
		return 1;
	}

	return CallWindowProcA( hook::original::fpWndProc, hWnd, uMsg, wParam, lParam );
}

uintptr_t hook::functions::DrawBarForUnit( uintptr_t rcx ) {
	const auto unit = *reinterpret_cast<C_DOTA_BaseNPC**>( rcx + 0x878 );

	//if ( !unit->IsAlly( ) && unit->IsHero( ) && pGui->HackBars ) return 0xFFFF;

	return hook::original::fpDrawBarForUnit( rcx );
}

void hook::logics::ReceivingInformation( ) {
	if ( global::inited ) {

		if ( global::g_iGameMode > EGameMode::INVALID && global::g_iGameState == EGameState::GAME_IN_PROGRESS || global::g_iGameState == EGameState::PRE_GAME && global::g_LocalController ) {
			const auto local_cont = reinterpret_cast<C_DOTAPlayerController*>( global::g_LocalController );
			if ( const auto local_assigned_hero = local_cont->GetAssignedHero( ); local_assigned_hero.IsValid( ) ) {

				global::g_LocalController = (uintptr_t*)local_cont;
				global::g_LocalEntity = g_pGameEntitySystem->find_entity<std::uintptr_t>( local_assigned_hero.GetIndex( ).Get( ) );
			}

			if ( global::g_LocalEntity ) {
				C_DOTA_BaseNPC* LocalEntity = reinterpret_cast<C_DOTA_BaseNPC*>( global::g_LocalEntity );

				for ( auto entity : cheat_data.UnitList ) {

					if ( !entity->ally( ) && !strcmp( entity->identity( )->GetName( ), "npc_dota_thinker" ) ) {
						bool exists = false;
						if ( entity->GetModifierManager( )->HasModifier( "modifier_invoker_sun_strike" ) ) {

							for ( const auto& tmp : cheat_data.ABILITYESP_LIST ) {
								if ( tmp.ability_ent == entity && tmp.ability == EAbilityType::Invoker_Sun_Strike ) exists = true;
							}

							if ( !exists ) {
								AbilityData_t abil;
								abil.ability = EAbilityType::Invoker_Sun_Strike;
								abil.ability_ent = entity;
								abil.create_time = global::g_flGameTime;

								cheat_data.ABILITYESP_LIST.push_back( abil );
							}
						}

						if ( entity->GetModifierManager( )->HasModifier( "modifier_lina_light_strike_array" ) ) {

							for ( const auto& tmp : cheat_data.ABILITYESP_LIST ) {
								if ( tmp.ability_ent == entity && tmp.ability == EAbilityType::Lina_Light_Strike_Array ) exists = true;
							}

							if ( !exists ) {
								AbilityData_t abil;
								abil.ability = EAbilityType::Lina_Light_Strike_Array;
								abil.ability_ent = entity;
								abil.create_time = global::g_flGameTime;

								cheat_data.ABILITYESP_LIST.push_back( abil );
							}
						}

						if ( entity->GetModifierManager( )->HasModifier( "modifier_kunkka_torrent_thinker" ) ) {

							for ( const auto& tmp : cheat_data.ABILITYESP_LIST ) {
								if ( tmp.ability_ent == entity && tmp.ability == EAbilityType::Kunkka_Torrent ) exists = true;
							}

							if ( !exists ) {
								AbilityData_t abil;
								abil.ability = EAbilityType::Kunkka_Torrent;
								abil.ability_ent = entity;
								abil.create_time = global::g_flGameTime;

								cheat_data.ABILITYESP_LIST.push_back( abil );
							}
						}
					}

					// перенёс в скрипт трусайт

					if ( entity->IsHero( ) ) {
						C_DOTA_BaseNPC_Hero* hero = static_cast<C_DOTA_BaseNPC_Hero*>( entity );

						if ( pGui->illusion_esp && hero->IsIllusion( ) ) {
							hero->SetIllusionManaBarVision( pGui->illusion_esp );
							hero->SetEntityColor( pGui->illusion_esp_r, pGui->illusion_esp_g, pGui->illusion_esp_b );
							if ( pGui->illusion_esp_can_select ) {
								if ( vector3d origin = hero->abs_origin( ); origin.z <= 39999 ) {
									origin.z = 39999;
									hero->set_abs_origin( origin );
								}
							}
						}

					}
				}
			}
		}
	}
}

void hook::logics::RenderInfo( ) {

	if ( global::inited ) {
		if ( global::g_iGameMode != EGameMode::INVALID && global::g_iGameState == EGameState::GAME_IN_PROGRESS || global::g_iGameState == EGameState::PRE_GAME ) {
			const auto& drawlist = ImGui::GetBackgroundDrawList( );

			cheat_data.PARTICLE_LIST_ITERATOR = cheat_data.PARTICLE_LIST.begin( );
			while ( cheat_data.PARTICLE_LIST_ITERATOR != cheat_data.PARTICLE_LIST.end( ) ) { // for крашит
				if ( cheat_data.PARTICLE_LIST_ITERATOR->second.create_time + 4.f < global::g_flGameTime || !cheat_data.PARTICLE_LIST_ITERATOR->second.attached_entity->identity( )->dormant( ) || cheat_data.PARTICLE_LIST_ITERATOR->second.attached_entity->health( ) <= 0 )
					cheat_data.PARTICLE_LIST_ITERATOR = cheat_data.PARTICLE_LIST.erase( cheat_data.PARTICLE_LIST_ITERATOR );
				else {
					vector2d screen_position;
					if ( !cheat_data.PARTICLE_LIST_ITERATOR->second.position.IsZero( ) && functions_helper::w2s( cheat_data.PARTICLE_LIST_ITERATOR->second.position, &screen_position ) ) {
						drawlist->AddImage( global::g_mapHeroIcons[cheat_data.PARTICLE_LIST_ITERATOR->second.attached_entity->identity( )->GetName( )],
											ImVec2{ screen_position.x, screen_position.y }, ImVec2{ screen_position.x + 35, screen_position.y + 35 } );
					}

					cheat_data.PARTICLE_LIST_ITERATOR++;
				}
			}

			cheat_data.PROJECTILE_LIST_ITERATOR = cheat_data.PROJECTILE_LIST.begin( );
			while ( cheat_data.PROJECTILE_LIST_ITERATOR != cheat_data.PROJECTILE_LIST.end( ) ) { // for крашит
				if ( cheat_data.PROJECTILE_LIST_ITERATOR->second.create_time + 4.f < global::g_flGameTime || !cheat_data.PROJECTILE_LIST_ITERATOR->second.attached_entity->identity( )->dormant( ) || cheat_data.PROJECTILE_LIST_ITERATOR->second.attached_entity->health( ) <= 0 )
					cheat_data.PROJECTILE_LIST_ITERATOR = cheat_data.PROJECTILE_LIST.erase( cheat_data.PROJECTILE_LIST_ITERATOR );
				else {
					vector2d screen_position;
					if ( !cheat_data.PROJECTILE_LIST_ITERATOR->second.position.IsZero( ) && functions_helper::w2s( cheat_data.PROJECTILE_LIST_ITERATOR->second.position, &screen_position ) ) {
						drawlist->AddImage( global::g_mapHeroIcons[cheat_data.PROJECTILE_LIST_ITERATOR->second.attached_entity->identity( )->GetName( )],
											ImVec2{ screen_position.x, screen_position.y }, ImVec2{ screen_position.x + 35, screen_position.y + 35 } );
					}

					cheat_data.PROJECTILE_LIST_ITERATOR++;
				}
			}

			cheat_data.ABILITYESP_LIST_ITERATOR = cheat_data.ABILITYESP_LIST.begin( );
			while ( cheat_data.ABILITYESP_LIST_ITERATOR != cheat_data.ABILITYESP_LIST.end( ) ) { // for крашит
				if ( cheat_data.ABILITYESP_LIST_ITERATOR->ability == EAbilityType::Invoker_Sun_Strike && cheat_data.ABILITYESP_LIST_ITERATOR->create_time + 1.7f < global::g_flGameTime ) {

					CDOTA_ParticleManager::GetInstance( ).DestroyParticle( "particles/units/heroes/hero_invoker/invoker_sun_strike_team.vpcf", cheat_data.ABILITYESP_LIST_ITERATOR->ability_ent );
					cheat_data.ABILITYESP_LIST_ITERATOR = cheat_data.ABILITYESP_LIST.erase( cheat_data.ABILITYESP_LIST_ITERATOR );
					continue;
				}

				if ( cheat_data.ABILITYESP_LIST_ITERATOR->ability == EAbilityType::Kunkka_Torrent && cheat_data.ABILITYESP_LIST_ITERATOR->create_time + 1.6f < global::g_flGameTime ) {

					CDOTA_ParticleManager::GetInstance( ).DestroyParticle( "particles/units/heroes/hero_kunkka/kunkka_spell_torrent_bubbles.vpcf", cheat_data.ABILITYESP_LIST_ITERATOR->ability_ent );
					cheat_data.ABILITYESP_LIST_ITERATOR = cheat_data.ABILITYESP_LIST.erase( cheat_data.ABILITYESP_LIST_ITERATOR );
					continue;
				}

				if ( cheat_data.ABILITYESP_LIST_ITERATOR->ability == EAbilityType::Lina_Light_Strike_Array && cheat_data.ABILITYESP_LIST_ITERATOR->create_time + 0.5f < global::g_flGameTime ) {

					CDOTA_ParticleManager::GetInstance( ).DestroyParticle( "particles/units/heroes/hero_lina/lina_spell_light_strike_array_ray_team.vpcf", cheat_data.ABILITYESP_LIST_ITERATOR->ability_ent );
					cheat_data.ABILITYESP_LIST_ITERATOR = cheat_data.ABILITYESP_LIST.erase( cheat_data.ABILITYESP_LIST_ITERATOR );
					continue;
				}

				else {
					vector2d vecw;
					if ( functions_helper::w2s( cheat_data.ABILITYESP_LIST_ITERATOR->ability_ent->abs_origin( ), &vecw ) ) {

						if ( cheat_data.ABILITYESP_LIST_ITERATOR->ability == EAbilityType::Invoker_Sun_Strike ) {
							CDOTA_ParticleManager::GetInstance( ).CreateParticle( "particles/units/heroes/hero_invoker/invoker_sun_strike_team.vpcf", ParticleAttachment_t::PATTACH_INVALID, reinterpret_cast<C_DOTA_BaseNPC*>( cheat_data.ABILITYESP_LIST_ITERATOR->ability_ent ) );
							drawlist->AddText( ImVec2{ vecw.x - 31, vecw.y + 10 }, 0xFFFFFFFF, std::format( "{:.2f}", cheat_data.ABILITYESP_LIST_ITERATOR->create_time + 1.7f - global::g_flGameTime ).c_str( ) );
						}
						else if ( cheat_data.ABILITYESP_LIST_ITERATOR->ability == EAbilityType::Kunkka_Torrent ) {
							CDOTA_ParticleManager::GetInstance( ).CreateParticle( "particles/units/heroes/hero_kunkka/kunkka_spell_torrent_bubbles.vpcf", ParticleAttachment_t::PATTACH_INVALID, reinterpret_cast<C_DOTA_BaseNPC*>( cheat_data.ABILITYESP_LIST_ITERATOR->ability_ent ) );
							drawlist->AddText( ImVec2{ vecw.x - 31, vecw.y + 10 }, 0xFFFFFFFF, std::format( "{:.2f}", cheat_data.ABILITYESP_LIST_ITERATOR->create_time + 1.6f - global::g_flGameTime ).c_str( ) );
						}
						else if ( cheat_data.ABILITYESP_LIST_ITERATOR->ability == EAbilityType::Lina_Light_Strike_Array ) {
							CDOTA_ParticleManager::GetInstance( ).CreateParticle( "particles/units/heroes/hero_lina/lina_spell_light_strike_array_ray_team.vpcf", ParticleAttachment_t::PATTACH_INVALID, reinterpret_cast<C_DOTA_BaseNPC*>( cheat_data.ABILITYESP_LIST_ITERATOR->ability_ent ) );
							drawlist->AddText( ImVec2{ vecw.x - 31, vecw.y + 10 }, 0xFFFFFFFF, std::format( "{:.2f}", cheat_data.ABILITYESP_LIST_ITERATOR->create_time + 0.5f - global::g_flGameTime ).c_str( ) );
						}
					}
				}

				cheat_data.ABILITYESP_LIST_ITERATOR++;
			}

			if ( cheat_data.m_flRoshanSpawned != -1 && cheat_data.m_flRoshanSpawned + 5.f > global::g_flGameTime ) {
				drawlist->AddText( ImVec2{ 150,50 }, 0xFFFFFFFF, "Roshan spawned!" );
			}
		}
	}
}