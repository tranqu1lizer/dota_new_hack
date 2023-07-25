#pragma once

#include "../definitions.h"

class CLocalize : public VClass {
	DEFINE_INTERFACE( CLocalize, "localize.dll", "Localize_001" );

	auto FindString( const std::string_view& str ) {
		return CallVFunc<16, const char*>( str.data( ) );
	}

	auto FindStringSafely( const std::string_view& str ) {
		return CallVFunc<17, const char*>( str.data( ) );
	}

	const wchar_t* ConvertANSIToUnicode( const char* str ) {
		return CallVFunc<18, const wchar_t*>( str );
	}
};