#pragma once

#define SPDLOG_USE_STD_FORMAT
#define SPDLOG_EOL ""

#include <windows.h>
#include <cstdint>
#include <vector>
#include <chrono>
#include <array>
#include <iostream>
#include <string_view>
#include <spdlog/spdlog.h>

#include "../sdk_game/valve/bitstream.hpp"
#include "undocumented.hpp"
#include "city.hpp"
#include "cityhash32.hxx"

enum entity_state : byte {
	state_updated = 0,
	state_leave,
	state_created,
	state_deleted
};

namespace util {
	std::string utf8_encode( const std::wstring& wstr );
	std::wstring utf8_decode( const std::string& str );

	void allocate_console( );
	void clear_console( );
	char* fast_strstr( const char* haystack, const char* needle );
	int fast_strcmp( char* s1, char* s2 );
	FARPROC find_export_address( HMODULE hModule, const char* lpProcName );
	void* memset( void* src, int val, unsigned __int64 count );
	int memcmp( const void* buf1,const void* buf2,size_t count );
	void* memcpy( void* dest, const void* src, unsigned __int64 count );
	void* find_game_system( const char* name );
	std::size_t fast_strlen( const char* str );

	template <typename T>
	void set_timer( T fn, int delay )
	{
		static unsigned long long tick_cout = 0;

		if ( GetTickCount64( ) - tick_cout >= delay ) {
			fn( );
			tick_cout = GetTickCount64( );
		}
	}

	void patch( void* address, const void* bytes );
	void patch( void* address, const void* bytes, int byteSize );
	char* fast_strcat( char* dest, const char* src );
	double Plat_FloatTime( );
	inline void set_console_color( int text = 0x7 /*ConColor::White*/, int background = 0x0 /*ConColor::Black*/ );
	bool exists( void* ptr ) noexcept;
	bool IsBadReadPtr( void* p );
	inline bool IsValidPtr( void* p );
	NTSTATUS set_virtual_protection( IN HANDLE process, IN OUT void** baseAddress, IN OUT std::uint32_t* size, IN std::uint32_t newProtection, OUT std::uint32_t* oldProtection );

	HINSTANCE get_module_base_wchar( const wchar_t* lpModuleName );
	HINSTANCE get_module_base_ansi( const char* lpModuleName );

	void* get_interface( const char* dllname, const char* interfacename );
	void* get_interface( HINSTANCE module_handle, const char* interfacename );

	uintptr_t vmt( uintptr_t region, int index );
	uintptr_t vmt1( uintptr_t region, int index );

	uintptr_t find_pattern( const char* module, const char* szSignature, const std::string_view& ="", bool format = true );
	uintptr_t find_pattern( HINSTANCE module, const char* szSignature, const std::string_view& ="", bool format = true );

	uintptr_t get_absolute_address( uintptr_t instruction_ptr, int offset, int size = -1 );
}