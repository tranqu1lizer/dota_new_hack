#include "overwolf.h"

std::size_t write_callback( void* contents, size_t size, size_t nmemb, void* userp ) {
	( (std::string*)userp )->append( (char*)contents, size * nmemb );
	return size * nmemb;
}

void COverWolf::process_lobby_members( ) {
	auto& gc = CGCClient::get( );
	if ( &gc && gc.lobby_manager( )->lobby_data ) {
		auto& dotaLobby = gc.lobby_manager( )->lobby_data->m_dota_lobby->get_dynamic_lobby( )->so_dynamic_lobby;
		CURL* curl = curl_easy_init( ); std::string url = "http://127.0.0.1:5000/?players=", read_buf; uint16_t pl_count = 0;
		curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, write_callback );
		curl_easy_setopt( curl, CURLOPT_WRITEDATA, &read_buf );

		for ( auto& member : dotaLobby.all_members( ) ) {
			if ( !member.has_id( ) || !member.has_name( ) ) continue;

			url += std::to_string( member.id( ) ) + ",";
			pl_count++;
		}
		if ( url[ url.size( ) - 1 ] == ',' ) url.pop_back( );
		url += "&count=" + std::to_string( pl_count );

		curl_easy_setopt( curl, CURLOPT_URL, url.c_str( ) );
		curl_easy_perform( curl );

		curl_easy_cleanup( curl );
	}
}

void COverWolf::process_member( const CSODOTALobbyMember& member, std::string* bf ) {

}