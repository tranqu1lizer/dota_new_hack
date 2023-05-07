#pragma once

#include "../functions/functions.hpp"

class CDOTARichPresence
{
public:
	static void SetRichPresence( const char* text )
	{
		static uintptr_t rich_presence = 0;
		if ( !rich_presence ) {
			auto vf = (std::uintptr_t)util::get_interface( global::client_module, "Source2Client002" );
			rich_presence = util::get_absolute_address( util::get_absolute_address( util::vmt( vf, 47 ) + 0x23, 1 ), 3 );
		}
		functions_call::SetRPStatus( rich_presence, text, 0, 0 ); // NOT WORK
	}
};