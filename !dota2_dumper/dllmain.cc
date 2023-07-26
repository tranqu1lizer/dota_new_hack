#include "sdk.hh"
#include <format>
#include <optional>
#include <fstream>
#include <set>
#include <algorithm>

const std::set<std::string> client_classes( {
	 "C_BaseEntity",
	 "CGameSceneNode",
	 "CBasePlayerController" ,
	 "C_DOTAPlayerController",
	 "C_DOTA_BaseNPC" ,
	 "C_DOTA_BaseNPC_Hero" ,
	 "CAttributeContainer" ,
	 "C_EconEntity" ,
	 "C_BaseCombatCharacter" ,
	 "CSkeletonInstance" ,
	 "C_BaseModelEntity" ,
	 "C_DOTAGamerules" ,
	 "CPlayer_CameraServices" ,
	"C_BasePlayerPawn" ,
	"C_DOTAGamerulesProxy" ,
} );

const std::set<std::string> server_classes( {
	"CDOTA_Buff"
 } );

HMODULE hmod;

void dump( ) {
	std::cout << "dump start\n\n";
	std::remove( "schema.h" );
	std::ofstream header{"schema.h"};

	header << "#pragma once\n\n#include <cstdint>\n\nnamespace schema {\n";

	for ( const auto& class_ : client_classes ) {
		static constexpr auto mod = "client.dll";

		const auto module_scope = CSchemaSystem::GetInstance( ).FindTypeScopeForModule( mod );

		if ( !module_scope )
			return;

		const auto class_info = module_scope->FindDeclaredClass( class_.c_str( ) );

		if ( !class_info )
			return;

		header << "    namespace " << class_.c_str( ) << " {\n";

		for ( auto k = 0; k < class_info->m_align; k++ ) {
			const auto& field = &class_info->m_fields[ k ];

			if ( !field || !field->m_single_inheritance_offset )
				continue;

			int sz;

			if ( !field->m_type->GetSize( &sz ) || !sz )
				continue;

			for ( auto j = 0; j < field->m_metadata_size; j++ ) {
				auto field_metadata = field->m_metadata[ j ];

				if ( !strcmp( field_metadata.m_name, "MNetworkChangeCallback" ) ) {
					header << "        // NetworkChangeCallback: " << field_metadata.m_value->m_sz_value << "\n";
				}
			}

			const auto str = std::format( "        constexpr std::uint16_t {} = {:#x}; // {} ({} bytes)\n", field->m_name, field->m_single_inheritance_offset, field->m_type->m_name_, sz );
			header << str;
		}
		header << "    }\n";
		std::cout << "dumped " << mod << "/" << class_ << "\n";
	}

	for ( const auto& class_ : server_classes ) {
		static constexpr auto mod = "server.dll";

		const auto module_scope = CSchemaSystem::GetInstance( ).FindTypeScopeForModule( mod );

		if ( !module_scope )
			return;

		const auto class_info = module_scope->FindDeclaredClass( class_.c_str( ) );

		if ( !class_info )
			return;

		header << "    namespace " << class_.c_str( ) << " {\n";

		for ( auto k = 0; k < class_info->m_align; k++ ) {
			const auto& field = &class_info->m_fields[ k ];

			if ( !field || !field->m_single_inheritance_offset )
				continue;

			int sz;

			if ( !field->m_type->GetSize( &sz ) || !sz )
				continue;

			for ( auto j = 0; j < field->m_metadata_size; j++ ) {
				auto field_metadata = field->m_metadata[ j ];

				if ( !strcmp( field_metadata.m_name, "MNetworkChangeCallback" ) ) {
					header << "        // NetworkChangeCallback: " << field_metadata.m_value->m_sz_value << "\n";
				}
			}
			const auto str = std::format( "        constexpr std::uint16_t {} = {:#x}; // {} ({} bytes)\n", field->m_name, field->m_single_inheritance_offset, field->m_type->m_name_, sz );
			header << str;
		}
		header << "    }\n";
		std::cout << "dumped " << mod << "/" << class_ << "\n";
	}
	header << "}\n";

	header.close( );

	std::cout << "\ndump end";
	FreeConsole( );
	FreeLibraryAndExitThread( hmod, 0 );
}

bool __stdcall DllMain( HINSTANCE hModule, std::uint8_t reason, void* ) {
	if ( reason == 1 ) {
		hmod = hModule;
		util::allocate_console( );
		setlocale( 0, "" );

		dump( );
	}
	return reason;
}