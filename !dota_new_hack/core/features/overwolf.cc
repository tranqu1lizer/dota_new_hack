#include "overwolf.h"

std::size_t write_callback( void* contents, size_t size, size_t nmemb, void* userp ) {
	( (std::string*)userp )->append( (char*)contents, size * nmemb );
	return size * nmemb;
}

void COverWolf::get_players_info( ) {
	auto& gc = CGCClient::get( );
	if ( gc.lobby_manager( )->lobby_data ) {
		CSODOTALobby& dotaLobby = gc.lobby_manager( )->lobby_data->m_dota_lobby->get_dynamic_lobby( )->so_dynamic_lobby;

		for ( auto& member : dotaLobby.all_members( ) ) {
			process_member( member );
			std::cout << "======================\n";
		}
	}
}

void COverWolf::process_member( const CSODOTALobbyMember& member ) {
	CURL* curl = curl_easy_init( ); CURLcode res; std::string read_buffer;

	std::wcout << "Member name: " << util::utf8_decode( proto_string( member.name( ) ) ) << "\n";
	if ( member.has_lane_selection_flags( ) ) std::cout << "    Lane Flags: " << member.lane_selection_flags( );

	curl_easy_setopt( curl, CURLOPT_URL, std::string{"https://api.opendota.com/api/players/" + std::to_string( (uint32_t)member.id( ) ) + "/"}.c_str( ) );
	curl_easy_setopt( curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L );
	curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, write_callback );
	curl_easy_setopt( curl, CURLOPT_WRITEDATA, &read_buffer );

	auto request = [&]( ) { return curl_easy_perform( curl ); };

	if ( std::async( std::launch::async, request ).get( ) != CURLE_OK ) return;
	curl_easy_cleanup( curl );
	nlohmann::json json_ = nlohmann::json::parse( read_buffer, nullptr, false );

	std::cout << "    MMR: " << json_[ "mmr_estimate" ][ "estimate" ] << "\n";

	process_member_heroes( (uint32_t)member.id( ) );
}

void COverWolf::process_member_heroes( std::uint32_t member_id ) {
	CURL* curl = curl_easy_init( ); CURLcode res; std::string read_buffer;
	auto request = [&]( ) { return curl_easy_perform( curl ); };

	curl_easy_setopt( curl, CURLOPT_URL, std::string{"https://api.opendota.com/api/players/" + std::to_string( member_id ) + "/heroes/"}.c_str( ) );
	curl_easy_setopt( curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L );
	curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, write_callback );
	curl_easy_setopt( curl, CURLOPT_WRITEDATA, &read_buffer );
	if ( std::async( std::launch::async, request ).get( ) != CURLE_OK ) return;
	curl_easy_cleanup( curl );

	nlohmann::json json_ = nlohmann::json::parse( read_buffer, nullptr, false );
	auto& hero_id = json_[0][ "hero_id" ];
	std::cout << "    Best hero: " << m_heroid[ hero_id ] << "\n";
}