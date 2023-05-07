#include "main.hpp"

#include "core/sdk_game/CDOTA_InventoryManager.hpp"

#include "gui/gui.hpp"
#include "core/hook/hook.hpp"
#include "core/lua/events.hpp"
#include "core/lua/scripts.hpp"
#include <fstream>

CGui* pGui = nullptr;
CGameEntitySystem* g_pGameEntitySystem = nullptr;
CDOTAInventoryManager* g_pInventoryManager = nullptr;
CDOTAGCClientSystem* g_pGCClientSystem = nullptr;

CheatData cheat_data;

inline const void FileSystemManipulation( ) {
	CBaseFileSystem::GetInstance( ).AddSearchPath( "C:\\ResourcesDHack", "DHK", SearchPathAdd_t::PATH_ADD_TO_TAIL, 1 );

	unsigned long prot;
	auto patch_addr = reinterpret_cast<void*>( &CBaseFileSystem::GetInstance( ).GetVMT().GetFunctionAt(118) ); //MarkContentCorrupt
	VirtualProtect( patch_addr, 3, 0x40, &prot );
	// 0x33, 0xC0 | xor eax, eax
	// 0xC3		  | ret
	util::memcpy( patch_addr, (void*)"\x33\xC0\xC3", 3 );
	VirtualProtect( patch_addr, 3, prot, nullptr );
}

int start_init( ) {
	std::cout << "Start init cheat..." << std::endl << std::endl;
	const auto start_init = std::chrono::high_resolution_clock::now( );

	global::client_module = util::get_module_base_wchar( L"client.dll" );
	if ( !global::client_module ) {
		std::cout << "client.dll base = nullptr" << std::endl;
		return 0;
	}
	FileSystemManipulation( );

	pGui = new CGui( );

	g_pInventoryManager = (CDOTAInventoryManager*)util::get_absolute_address( util::find_pattern( global::client_module, "48 8B 05 ? ? ? ? 48 8D 15 ? ? ? ? 48 89 0D ? ? ? ? 48 83 C4 ? 48 FF A0 50 01 00 00 CC CC CC CC CC CC CC CC CC 48 83 EC ? 80 64 24 38", "CDOTAInventoryManager" ), 3, 7 );
	const auto GetSteamPipe = reinterpret_cast<HSteamPipe( * )( )>( util::find_export_address( util::get_module_base_ansi( "steam_api64.dll" ), "GetHSteamPipe", true ) );
	const auto GetSteamUser = reinterpret_cast<HSteamUser( * )( )>( util::find_export_address( util::get_module_base_ansi( "steam_api64.dll" ), "GetHSteamUser", true ) );

	// xxx????xxxxx
	const auto gcclient_pattern = util::find_pattern( global::client_module, "\x48\x8D\x0D\xCC\xCC\xCC\xCC\x48\x0F\x45\xD0\xE8", "s_DOTAGCClientSystem", false );
	if ( !gcclient_pattern ) return MessageBoxA( 0, "gcclient_pattern = nullptr", 0, MB_ICONERROR );

	g_pGCClientSystem = (CDOTAGCClientSystem*)util::get_absolute_address( gcclient_pattern, 3, 7 );
	global::g_CGCClient = *reinterpret_cast<uintptr_t*>( util::get_absolute_address( util::find_pattern( global::client_module, "48 8B 05 ? ? ? ? 45 33 F6 48 8B 7B 30", "CGCClient" ), 3, 7 ) );

	const auto ui_engine = (uintptr_t)CPanoramaUIEngine::GetInstance( ).m_pUIEngineSource2;

	global::patterns::ColoredBarCreeps = util::find_pattern( global::client_module, "\xFF\x52\x20\xF3\x0F\x10\x35", "UnitBarContrast", false );
	if ( !global::patterns::ColoredBarCreeps ) return MessageBoxA( 0, "g_ColoredBarCreeps_pattern failed", 0, MB_ICONERROR );

	const auto aGameEntitySystem = util::vmt((std::uintptr_t)util::get_interface( global::client_module, "Source2Client002" ), 25);
	if ( !aGameEntitySystem ) return MessageBoxA( 0, "GameEntitySystem = nullptr", 0, MB_ICONERROR );
	g_pGameEntitySystem = *reinterpret_cast<CGameEntitySystem**>( util::get_absolute_address( aGameEntitySystem, 3, 7 ) );

	const auto aParticleManager = util::get_absolute_address( util::find_pattern( global::client_module, "7E 0B 41 8D 42 F1 A9 FB FF FF FF 75 0B 41 8B C9 E8 ? ? ? ? 48 8B D8 E8", "ParticleManager" ) + 0x18, 1, 5 );
	if ( !aParticleManager ) return MessageBoxA( 0, "aParticleManager sig = nullptr", 0, MB_ICONERROR );
	global::patterns::DOTAParticleManager = util::get_absolute_address( aParticleManager, 3, 7 );

	const auto aGameRules = util::find_pattern( global::client_module, "48 8B 3D ? ? ? ? 8B DA 48 8B E9", "CGameRules" );
	if ( !aGameRules ) return MessageBoxA( 0, "aGameRules sig = nullptr", 0, MB_ICONERROR );
	global::patterns::GameRules = util::get_absolute_address( aGameRules, 3, 7 );


	const auto start_funcs = std::chrono::high_resolution_clock::now( );
	/*
	* Get functions
	*/
	{
		global::patterns::CImagePanel__SetImage =
			util::get_absolute_address(util::find_pattern( "client.dll", "E8 ? ? ? ? EB 18 4E 8B 04 37", "SetImage" ), 1, 5 );
		global::patterns::CPanel2D__DeleteAsync =
			util::get_absolute_address(util::find_pattern( "client.dll", "\xE8\xCC\xCC\xCC\xCC\x4C\x89\x7F\x60", "CPanel2D::DeleteAsync", false ), 1, 5 );
		global::patterns::CTextEntry__SetText =
			util::get_absolute_address(util::find_pattern( "client.dll", "E8 ? ? ? ? 48 8B 77 10", "CTextEntry::SetText" ), 1, 5 );
		global::patterns::CDOTA_UI_AbilityImage__SetAbilityName =
			util::get_absolute_address(util::find_pattern( "client.dll", "E8 ? ? ? ? 4A 8B 04 E7", "CDOTA_UI_AbilityImage::SetAbilityName" ), 1, 5 );

		functions_call::g_pWorldToScreen = reinterpret_cast<functions_prototype::WorldToScreen_t>( 
			util::get_absolute_address( util::find_pattern( global::client_module, "\xE8\xCC\xCC\xCC\xCC\x39\x6C\x24\x68", "WorldToScreen", false ),1,5 ));
		functions_call::g_pOnColorChanged = reinterpret_cast<functions_prototype::OnColorChanged_t>( 
			util::find_pattern( global::client_module, "40 53 48 83 EC 20 48 8B D9 48 8B 89 ? ? ? ? 48 8B 01 0F B6 93", "OnColorChanged" ) );
		functions_call::SendReadyUpMessage = reinterpret_cast<functions_prototype::CDOTAGCClientSystem__SendReadyUpMessageForCurrentLobby>(
			util::find_pattern( global::client_module, "48 89 5C 24 08 48 89 74 24 10 57 48 81 EC ? ? ? ? B9", "SendReadyUpMessage" ));
		functions_call::ConsoleColoredMessage = reinterpret_cast<functions_prototype::ConColorMsg>(GetProcAddress(
			util::get_module_base_ansi( "tier0.dll" ), "?ConColorMsg@@YAXAEBVColor@@PEBDZZ" ));
		functions_call::ConsoleMessage = reinterpret_cast<functions_prototype::ConMsg>(GetProcAddress( 
			util::get_module_base_ansi( "tier0.dll" ), "Msg" ));
		functions_call::CreateEconItem = reinterpret_cast<functions_prototype::CreateEconItem_t>(
			util::find_pattern( global::client_module, "48 83 EC ? B9 ? ? ? ? E8 ? ? ? ? 48 85 C0 74 ? 48 8D 0D", "CreateEconItem" ));
		functions_call::GetCurrentCamera = reinterpret_cast<functions_prototype::GetDOTACamera_t>(
			util::get_absolute_address( util::find_pattern( global::client_module, "E8 ? ? ? ? 48 85 C0 74 ? C6 80 84 02 00 00", "DOTACameraInit" ),1,5));
		functions_call::GetSOCDataForItem = reinterpret_cast<functions_prototype::GetSOCDataForItem_t>(
			util::find_pattern( global::client_module, "48 89 54 24 ? 57 48 83 EC 70", "GetSOCDataForItem" ));
		functions_call::DestroyParticle = reinterpret_cast<functions_prototype::DestroyParticle_t>(
			util::find_pattern( global::client_module, "83 FA ? 0F 84 ? ? ? ? 48 89 6C 24 18", "DestroyParticle" ));
		functions_call::GetLevelSpecialValueFor = reinterpret_cast<functions_prototype::GetLevelSpecialValueFor_t>(
			util::get_absolute_address(util::find_pattern( global::client_module, "E8 ? ? ? ? F3 0F 5A C0", "FindLevelSpecialValueFor" ),1,5));
		functions_call::GetGameTime = reinterpret_cast<functions_prototype::GetGameTime_t>(
			util::get_absolute_address(util::find_pattern( global::client_module, "E8 ? ? ? ? 8B 04 2E", "GetGameTime" ),1,5));
		functions_call::GetPlayer = reinterpret_cast<functions_prototype::GetPlayer_t>(
			util::find_pattern( global::client_module, "\x33\xC0\x83\xF9\xFF", "GetPlayer", false ));

		if ( !functions_call::GetLevelSpecialValueFor ) return MessageBoxA( 0, "GetLevelSpecialValueFor not found", 0, MB_ICONERROR );
		if ( !functions_call::GetCurrentCamera ) return MessageBoxA( 0, "GetCameraManager not found", 0, MB_ICONERROR );
		if ( !functions_call::g_pOnColorChanged ) return MessageBoxA( 0, "OnColorChanged sig = nullptr", 0, MB_ICONERROR );
		if ( !functions_call::SendReadyUpMessage ) return MessageBoxA( 0, "SendReadyUpMessage sig = nullptr", 0, MB_ICONERROR );
		if ( !functions_call::g_pWorldToScreen ) return MessageBoxA( 0, "WorldToScreen sig = nullptr", 0, MB_ICONERROR );
		if ( !functions_call::CreateEconItem ) return MessageBoxA( 0, "CreateEconItem = nullptr", 0, MB_ICONERROR );
		if ( !functions_call::DestroyParticle ) return MessageBoxA( 0, "DestroyParticle = nullptr", 0, MB_ICONERROR );
		if ( !functions_call::GetGameTime ) return MessageBoxA( 0, "GetGameTime = nullptr", 0, MB_ICONERROR );
		if ( !functions_call::GetPlayer ) return MessageBoxA( 0, "GetPlayer = nullptr", 0, MB_ICONERROR );
	}

	const auto stop_funcs = std::chrono::high_resolution_clock::now( );
	const auto duration_funcs = duration_cast<std::chrono::milliseconds>( stop_funcs - start_funcs );

	const auto start_hooks = std::chrono::high_resolution_clock::now( );
	/*
	* Hooks
	*/
	{
		const auto steam_interface = 
			reinterpret_cast<uintptr_t>(ISteamClient::GetInstance( ).GetISteamGenericInterface( GetSteamUser( ), GetSteamPipe( ), "SteamGameCoordinator001" ));
		const auto aDrawBarForUnit = 
			util::find_pattern( global::client_module, "\x8B\x41\x50\x83\xF8", "DrawBarForUnit", false );
		const auto aBAsyncSendProto = 
			util::get_absolute_address( util::find_pattern( global::client_module, "E8 ? ? ? ? 48 8B 4D 90 48 89 7C 24", "BAsyncSendProto" ), 1, 5);
		const auto aDispatchPacket =
			util::get_absolute_address( util::find_pattern( global::client_module, "E8 ? ? ? ? 48 8B CF E8 ? ? ? ? B8", "DispatchPacket" ), 1, 5);
		const auto aPrepareUnitOrders = 
			util::get_absolute_address( util::find_pattern( global::client_module, "E8 ? ? ? ? F3 44 0F 11 5B", "PrepareUnitOrders" ), 1, 5 );
		const auto aPresent = 
			util::find_pattern( "GameOverlayRenderer64.dll", "48 89 6C 24 18 48 89 74 24 20 41 56 48 83 EC 20 41 8B E8", "SteamOverlayPresent" );
		const auto aFireEventClientSide = 
			util::find_pattern( global::client_module, "48 89 5C 24 20 57 41 54 41 56 48 83 EC ? 4C 8B F2", "FireEventClientSide" );

		if ( !aDrawBarForUnit ) return MessageBoxA( 0, "DrawBarForUnit failed", 0, MB_ICONERROR );
		if ( !aPrepareUnitOrders ) return MessageBoxA( 0, "aPrepareUnitOrders error", 0, MB_ICONERROR );
		if ( !aPresent ) return MessageBoxA( 0, "aPresent error", 0, MB_ICONERROR );
		if ( !aFireEventClientSide ) return MessageBoxA( 0, "aFireEventClientSide error", 0, MB_ICONERROR );

		std::cout << std::endl;

		hook::install_hook( aPrepareUnitOrders, &hook::functions::PrepareUnitOrders, reinterpret_cast<LPVOID*>( &hook::original::fpPrepareUnitOrders ), "PrepareUnitOrders" );
		hook::install_hook( aPresent, &hook::functions::Present, reinterpret_cast<LPVOID*>( &hook::original::fpPresent ), "SteamOverlayPresent" );
		hook::install_hook( aFireEventClientSide, &hook::functions::FireEventClientSide, reinterpret_cast<LPVOID*>( &hook::original::fpFireEventClientSide ), "FireEventClientSide" );
		hook::install_hook( util::vmt( (uintptr_t)ui_engine, 0x6 ), &hook::functions::RunFrame, reinterpret_cast<LPVOID*>( &hook::original::fpRunFrame ), "RunFrame" );
		hook::install_hook( aDrawBarForUnit, &hook::functions::DrawBarForUnit, reinterpret_cast<LPVOID*>( &hook::original::fpDrawBarForUnit ), "DrawBarForUnit" );
		hook::install_hook( util::vmt( (uintptr_t)&CNetworkSystem::GetInstance( ), 26 ), &hook::functions::CreateNetChannel, reinterpret_cast<LPVOID*>( &hook::original::fpCreateNetChannel ), "CreateNetChannel" );
		hook::install_hook( util::vmt( steam_interface, 0 ), &hook::functions::ISteamGameCoordinator__SendMessage, reinterpret_cast<LPVOID*>( &hook::original::fpSClientSendMessage ), "SendMessage" );
		hook::install_hook( util::vmt( steam_interface, 1 ), &hook::functions::ISteamGameCoordinator__IsMessageAvailable, reinterpret_cast<LPVOID*>( &hook::original::fpSClientIsMessageAvailable ), "IsMessageAvailable" );
		hook::install_hook( util::vmt( steam_interface, 2 ), &hook::functions::ISteamGameCoordinator__RetrieveMessage, reinterpret_cast<LPVOID*>( &hook::original::fpSClientRetrieveMessage ), "RetrieveMessage" );
		hook::install_hook( aBAsyncSendProto, &hook::functions::BAsyncSendProto, reinterpret_cast<LPVOID*>( &hook::original::fpBAsyncSendProto ), "BAsyncSendProto" );
		hook::install_hook( aDispatchPacket, &hook::functions::DispatchPacket, reinterpret_cast<LPVOID*>( &hook::original::fpDispatchPacket ), "DispatchPacket" );
		hook::install_hook( util::vmt( (uintptr_t)g_pGameEntitySystem, 14 ), &hook::functions::OnAddEntity, reinterpret_cast<LPVOID*>( &hook::original::fpOnAddEntity ), "OnAddEntity" );
		hook::install_hook( util::vmt( (uintptr_t)g_pGameEntitySystem, 15 ), &hook::functions::OnRemoveEntity, reinterpret_cast<LPVOID*>( &hook::original::fpOnRemoveEntity ), "OnRemoveEntity" );

		// hook::install_hook( util::vmt( ui_engine, 88 ), &hook::functions::RunScript, reinterpret_cast<LPVOID*>( &hook::original::fpRunScript ), "RunScript" );
	}
	const auto stop_hooks = std::chrono::high_resolution_clock::now( );
	const auto duration_hooks = duration_cast<std::chrono::milliseconds>( stop_hooks - start_hooks );

	std::jthread( []
				  {
		while ( !pGui->pDevice ) {
			std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
		}
		/*for ( const auto& spellicons_dir : std::filesystem::directory_iterator( "C:\\ResourcesDHack\\icon\\spells" ) ) {

			auto filename = spellicons_dir.path( ).filename( ).string( );
			filename = filename.substr( 0, filename.find( "_png" ) );
	
			pGui->LoadTextureFromFile( spellicons_dir.path( ).string( ), &global::g_mapSpellIcons[filename] );
		}

		for ( const auto& heroicons_dir : std::filesystem::directory_iterator( "C:\\ResourcesDHack\\icon\\heroes" ) ) {

			auto filename = heroicons_dir.path( ).filename( ).string( );
			filename = filename.substr( 0, filename.find( "_png" ) );

			pGui->LoadTextureFromFile( heroicons_dir.path( ).string( ), &global::g_mapHeroIcons[filename] );
		}*/

		//for ( const auto& itemicons_dir : std::filesystem::directory_iterator( "C:\\ResourcesDHack\\icon\\items" ) ) {
		//			auto filename = itemicons_dir.path( ).filename( ).string( );
		//			filename = filename.substr( 0, filename.find( "_png" ) );
		//			if ( pGui->LoadTextureFromFile( itemicons_dir.path( ).string( ), &global::g_mapItemIcons[filename] ) ) {
		//			}
		//		}

		for ( const auto& scripts_dir : std::filesystem::directory_iterator( CLuaScript::k_path_scripts ) ) {
			if ( scripts_dir.path( ).filename( ).extension( ) == ".lua" || scripts_dir.path( ).filename( ).extension( ) == ".luac" )
				lua_scripts_list.push_back( CLuaScript::load_script_file( scripts_dir.path( ).filename( ).string( ) ) );
		}

		global::inited = true;
		pGui->show = true;
	} ).detach( );

	CGameUIService::GetInstance( ).ShowConsole( true );

	std::cout << std::endl << "Init functions " << "took " << std::dec << duration_funcs.count( ) << "ms" << std::endl;
	std::cout << "Init hooks " << "took " << std::dec << duration_hooks.count( ) << "ms" << std::endl;
	const auto duration_init = duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now( ) - start_init );
	std::cout << "Full cheat initialization " << "took " << std::dec << duration_init.count( ) << "ms" << std::endl;
	return true;
}

bool __stdcall DllMain( HINSTANCE hModule, std::uint8_t reason, void* ) {
	if ( reason == 1 ) {
		srand( time( NULL ) );
		util::allocate_console( );
		util::clear_console( );
		global::hModule = hModule;
		start_init( );
	}
	return reason;
}