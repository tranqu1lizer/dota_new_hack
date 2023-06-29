#define FIND_FUNC_CLIENT( fn, sig, fn_name ) fn = (decltype(fn))util::find_pattern( global::client, sig, fn_name ); \
if(!fn) return MessageBoxA( 0, fn_name, 0, MB_ICONERROR );

#define FIND_FUNC_CLIENT_CALL( fn, sig, fn_name ) fn = (decltype(fn))util::get_absolute_address(util::find_pattern( global::client, sig, fn_name ), 1, 5); \
if(!fn) return MessageBoxA( 0, fn_name, 0, MB_ICONERROR );

#define DETOUR_PATTERN( dll_name, sig, sig_name, hk_name, fm, relative_call ) \
hook::install_hook(relative_call ? util::get_absolute_address(util::find_pattern( dll_name, sig, sig_name, fm),1,5) : util::find_pattern( dll_name, sig, sig_name, fm), &hook::functions::##hk_name, &hook::original::fp##hk_name, #hk_name )

#define DETOUR_VF( ptr, index, hk_name, is_vmt ) \
hook::install_hook(is_vmt ? util::vmt1((uintptr_t)ptr, index) : util::vmt((uintptr_t)ptr, index), &hook::functions::##hk_name, &hook::original::fp##hk_name, #hk_name )

#define GAB( var, offset, sz ) util::get_absolute_address(var, offset,sz)

#define CHECK_VAR( var ) \
if(!var) {\
spdlog::critical( "{} is nullptr", #var );\
return;\
}

#define CHECK_VAR_RET( var ) \
if(!var) {\
spdlog::critical( "{} is nullptr", #var );\
return 0;\
}

#include "main.hpp"

#include "core/sdk_game/CDOTA_InventoryManager.hpp"

#include "gui/gui.hpp"
#include "core/hook/hook.hpp"
#include "gui/panorama_gui.h"

CGui* pGui = nullptr;
CGameEntitySystem* g_pGameEntitySystem = nullptr;

inline const void some_memory_patches( ) {
	CBaseFileSystem::GetInstance( ).AddSearchPath( "C:\\ResourcesDHack", "DHK", SearchPathAdd_t::PATH_ADD_TO_TAIL, 1 );

	void* patch_address;

	{
		patch_address = (void*)CBaseFileSystem::GetInstance( ).GetVF( 118 ); // MarkContentCorrupt
		// 0x33, 0xC0 | xor eax, eax
		// 0xC3		  | ret
		util::patch( patch_address, "\x33\xC0\xC3" );
	}

	{
		// const auto patch_address = reinterpret_cast<void*>( util::find_pattern( global::client_module, "BF ? ? ? ? E8 ? ? ? ? 83 F8 01", "MatchmakingStats interval" ) );
		// 0xBF, 0x08, 0x00, 0x00, 0x00 | mov edi, 8
		// util::patch( patch_address, "\xBF\x08\x00\x00\x00", 5 );
	}

	{
		patch_address = (void*)util::find_pattern( "panorama.dll", "FF 15 ? ? ? ? 48 8B 7C 24 ? 41 83 FE", "fill-parent-flow console flood" );

		util::patch( patch_address, "\x90\x90\x90\x90\x90\x90" );

		patch_address = (void*)util::find_pattern( "panorama.dll", "FF 15 ? ? ? ? 41 83 FC", "fill-parent-flow console flood2" );

		util::patch( patch_address, "\x90\x90\x90\x90\x90\x90" );
	}
}

bool start_init( ) {
	std::uintptr_t aGameEntitySystem,
		aParticleManager,
		aNetchanVMT,
		aSteamGC;
	std::uintptr_t** aParticleCollectionVMT;
	spdlog::stopwatch start_init;

	spdlog::debug( "Start init cheat...\n\n" );

	const auto peb = reinterpret_cast<PPEB64>( __readgsqword( 0x60 ) );
	peb->BeingDebugged = 0;

	spdlog::info( "PEB: {} | IsDebuggerPresent(): {}\n", (void*)peb, IsDebuggerPresent( ) ? "true" : "false" );

	global::client = util::get_module_base_wchar( L"client.dll" );
	CHECK_VAR_RET( global::client );

	some_memory_patches( );
	pGui = new CGui( );

	{
		aGameEntitySystem = util::vmt( (std::uintptr_t)util::get_interface( global::client, "Source2Client002" ), 25 );
		CHECK_VAR_RET( aGameEntitySystem );

		aParticleManager = GAB( util::find_pattern( global::client, "7E 0B 41 8D 42 F1 A9 FB FF FF FF 75 0B 41 8B C9 E8 ?? ?? ?? ?? 48 8B D8 E8", "CDOTA_ParticleManager" ) + 0x18, 1, 5 );
		CHECK_VAR_RET( aParticleManager );

		aNetchanVMT = util::find_pattern( "networksystem.dll", "40 53 56 57 41 56 48 83 EC ?? 45 33 F6 48 8D 71", "NetChannel VMT" );
		CHECK_VAR_RET( aNetchanVMT )

			aParticleCollectionVMT = (uintptr_t**)GAB( util::find_pattern( "particles.dll", "48 8D 05 ?? ?? ?? ?? 48 89 01 0F 57 C0", "ParticleCollection VMT" ), 3, 7 );
		CHECK_VAR_RET( aParticleCollectionVMT );

		g_pGameEntitySystem = *reinterpret_cast<CGameEntitySystem**>( GAB( aGameEntitySystem, 3, 7 ) );
		global::patterns::DOTAParticleManager = GAB( aParticleManager, 3, 7 );
	}

	spdlog::stopwatch start_funcs;
	/*
	* Get functions
	*/
	{
		FIND_FUNC_CLIENT( global::patterns::CSlider__SetValue, "40 57 48 83 EC ?? 0F 29 74 24 ?? 48 8B F9 F3 0F 10 71", "CPanel2D::SetValue" );
		FIND_FUNC_CLIENT( global::patterns::CDOTA_UI_HeroImage__SetHeroName, "48 89 5C 24 ?? 57 48 83 EC 20 48 8B FA 48 8B D9 E8 ?? ?? ?? ?? 80 B8 ?? ?? ?? ?? ?? 75 17 80 B8 ?? ?? ?? ?? ?? 75 0E 80 B8 ?? ?? ?? ?? ?? B9 ?? ?? ?? ?? 74 05 B9 ?? ?? ?? ?? 48 03 C8 48 8B D7 E8 ?? ?? ?? ?? 39 83", "CDOTA_UI_HeroImage::SetHeroName" );

		FIND_FUNC_CLIENT_CALL( global::patterns::CEconItem__DeserializeItemProtobuf, "E8 ?? ?? ?? ?? 41 0F B6 46 ?? A8 01", "CEconItem::DeserializeFromProtoBufItem" );
		FIND_FUNC_CLIENT_CALL( global::patterns::CImagePanel__SetImage, "E8 ?? ?? ?? ?? EB 18 4E 8B 04 37", "CImagePanel::SetImage" );
		FIND_FUNC_CLIENT_CALL( global::patterns::CPanel2D__DeleteAsync, "E8 ?? ?? ?? ?? 4C 89 7F 60", "CPanel2D::DeleteAsync" );
		FIND_FUNC_CLIENT_CALL( global::patterns::CTextEntry__SetText, "E8 ?? ?? ?? ?? 48 8B 77 10", "CTextEntry::SetText" );
		FIND_FUNC_CLIENT_CALL( global::patterns::CTextEntry__GetText, "E8 ?? ?? ?? ?? F3 0F 10 5D ?? 48 8B D0", "CTextEntry::GetText" );
		FIND_FUNC_CLIENT_CALL( global::patterns::CDOTA_UI_AbilityImage__SetAbilityName, "E8 ?? ?? ?? ?? 4A 8B 04 E7", "CDOTA_UI_AbilityImage::SetAbilityName" );
		FIND_FUNC_CLIENT_CALL( global::patterns::CDOTA_UI_ItemImage__SetItemByName, "E8 ?? ?? ?? ?? E9 ?? ?? ?? ?? 48 8B D3 48 8B C8", "CDOTA_UI_ItemImage::SetItemByName" );

		FIND_FUNC_CLIENT( calls::GetSOCDataForItem, "48 89 54 24 ?? 57 48 83 EC 70", "CDOTAPlayerInventory::GetSOCDataForItem" );
		FIND_FUNC_CLIENT( calls::destroy_particle, "83 FA ?? 0F 84 ?? ?? ?? ?? 48 89 6C 24 18", "CDOTA_ParticleManager::DestroyParticle" );
		FIND_FUNC_CLIENT( calls::CreateEconItemObject, "48 83 EC ?? B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 85 C0 74 ?? 48 8D 0D", "CreateEconItem" );

		FIND_FUNC_CLIENT_CALL( calls::GetCurrentCamera, "E8 ?? ?? ?? ?? 48 85 C0 74 ?? C6 80 84 02 00 00", "C_DOTACameraInit" );
		FIND_FUNC_CLIENT_CALL( calls::GetLevelSpecialValueFor, "E8 ?? ?? ?? ?? F3 0F 5A C0", "C_BaseEntity::FindLevelSpecialValueFor" );
		FIND_FUNC_CLIENT_CALL( calls::CDOTA_Hud_Top_Bar__UpdateNetWorthDifference, "E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B C8 E8 ?? ?? ?? ?? 84 C0 0F 84 ?? ?? ?? ?? E8", "CDOTA_Hud_Top_Bar::UpdateNetWorthDifference" );
		FIND_FUNC_CLIENT_CALL( CDOTAItemSchema::GetItemDefByIndex, "E8 ?? ?? ?? ?? 8B 4E 64", "CDOTAItemSchema::GetItemDefByIndex" );

		FIND_FUNC_CLIENT_CALL( C_DOTA_PlayerResource::aGetNetWorthOfPlayer, "E8 ?? ?? ?? ?? F3 0F 5F F7", "C_DOTA_PlayerResource::GetNetWorthOfPlayer" );
		FIND_FUNC_CLIENT_CALL( C_DOTA_PlayerResource::aGetLastHits, "E8 ?? ?? ?? ?? 3B F0 7C 5A", "C_DOTA_PlayerResource::GetLastHits" );
		FIND_FUNC_CLIENT_CALL( C_DOTA_PlayerResource::aGetDenies, "E8 ?? ?? ?? ?? 45 8B C6 89 44 24 44", "C_DOTA_PlayerResource::GetLastHits" );
		FIND_FUNC_CLIENT_CALL( CPanel2D::aSetDialogVariableInt, "E8 ?? ?? ?? ?? 8B 53 5C", "CPanel2D::SetDialogVariableInt" );
		FIND_FUNC_CLIENT_CALL( CDropDown::aCDropDown__GetSelected, "E8 ?? ?? ?? ?? 48 85 C0 0F 84 ?? ?? ?? ?? 49 8B 8F", "CDropDown::GetSelected" );
		
		// E8 ? ? ? ? EB 4E 83 FB FF

		CDOTAItemSchema::GetItemDefArrIdx = AddressWrapper( CDOTAItemSchema::GetItemDefByIndex ).get_offset( 0x16 ).get_address_from_instruction_ptr( 1 );
		ISteamClient::GetHSteamPipe = (decltype( ISteamClient::GetHSteamPipe ))util::find_export_address( util::get_module_base_ansi( "steam_api64.dll" ), "GetHSteamPipe" );
		ISteamClient::GetHSteamUser = (decltype( ISteamClient::GetHSteamUser ))util::find_export_address( util::get_module_base_ansi( "steam_api64.dll" ), "GetHSteamUser" );
	}

	const auto duration_funcs = start_funcs.elapsed( );

	spdlog::stopwatch start_hooks;
	/*
	* Hooks
	*/
	{
		aSteamGC = (std::uintptr_t)ISteamClient::get( ).GetISteamGenericInterface( ISteamClient::GetHSteamPipe( ), ISteamClient::GetHSteamUser( ), "SteamGameCoordinator001" );
		CHECK_VAR_RET( aSteamGC );

		DETOUR_PATTERN( "client.dll", "E8 ?? ?? ?? ?? 48 8B 4D 90 48 89 7C 24", "CGCClient::BAsyncSendProto", BAsyncSendProto, true, true );
		DETOUR_PATTERN( "GameOverlayRenderer64.dll", "48 89 6C 24 ?? 48 89 74 24 ?? 41 56 48 83 EC ?? 41 8B E8", "SteamOverlayPresent", Present, true, false );

		DETOUR_VF( GAB( aNetchanVMT + 0x15, 3, 7 ), 86, PostReceivedNetMessage, true );
		DETOUR_VF( GAB( aNetchanVMT + 0x15, 3, 7 ), 69, SendNetMessage, true );
		DETOUR_VF( aSteamGC, 2, SGCRetrieveMessage, false );

		g_pEntityListener = CMemAlloc::GetInstance( )->allocate<EntityEventListener>( );
		g_pGameEntitySystem->entity_listeners( ).add_to_tail( g_pEntityListener );
	}

	for ( auto& ccmd : ICVar::get( ).ccommands( ) ) {

		if ( !( &ccmd ) || !ccmd.m_name )
			continue;

		if ( ccmd.m_flags & FCVAR_DEVELOPMENTONLY )
			ccmd.m_flags &= ~FCVAR_DEVELOPMENTONLY;

		if ( ccmd.m_flags & FCVAR_HIDDEN )
			ccmd.m_flags &= ~FCVAR_HIDDEN;

		if ( ccmd.m_flags & FCVAR_CHEAT )
			ccmd.m_flags &= ~FCVAR_CHEAT;
	}

	for ( auto& [cvar_node, idx] : ICVar::get( ).cvars( ) )
	{
		if ( !cvar_node || !cvar_node->m_name )
			continue;

		if ( cvar_node->m_flags & FCVAR_DEVELOPMENTONLY )
			cvar_node->m_flags &= ~FCVAR_DEVELOPMENTONLY;

		if ( cvar_node->m_flags & FCVAR_HIDDEN )
			cvar_node->m_flags &= ~FCVAR_HIDDEN;

#ifdef _DEBUG
		cvar_node->m_flags &= FCVAR_NOTIFY;
#endif
	}

	std::cout << "\n";
	spdlog::debug( "Unlocked all console variables/commands\n\n" );

	spdlog::info( "Init functions took {:.2}s\n", duration_funcs.count( ) );
	spdlog::info( "Init hooks took {:.2}s\n", start_hooks );
	spdlog::info( "Full cheat initialization took {:.2}s\n\n", start_init );

	return true;
}

bool __stdcall DllMain( HINSTANCE hModule, std::uint8_t reason, void* ) {
	if ( reason == 1 ) {
		util::allocate_console( );
		util::clear_console( );
		setlocale( 0, "" );
		global::hModule = hModule;

#ifdef _DEBUG
		spdlog::set_level( spdlog::level::debug );
#endif

		start_init( );
	}
	return reason;
}