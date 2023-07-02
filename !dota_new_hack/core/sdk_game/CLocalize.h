#pragma once

class CLocalize : public VClass {
	static auto GetInstanceImpl( )
	{
		static CLocalize* inst = nullptr;
		if ( !inst ) inst = static_cast<CLocalize*>( util::get_interface( "localize.dll", "Localize_001" ) );

		return inst;
	}
public:
	static auto& GetInstance( )
	{
		return *GetInstanceImpl( );
	}

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