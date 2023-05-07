#pragma once

#include <windows.h>
#include <cstdint>
#include <vector>
#include <chrono>
#include <array>
#include <iostream>
#include <string_view>

#include "city.hpp"
#include "cityhash32.hxx"

namespace util {
	std::wstring ConvertANSIToWide( const std::string& str );
	std::string ConvertWideToANSI( const std::wstring& wstr );

	void allocate_console( );
	void clear_console( );
	char* fast_strstr( const char* haystack, const char* needle );
	int fast_strcmp( char* s1, char* s2 );
	FARPROC __stdcall find_export_address( _In_ HMODULE hModule, _In_ LPCSTR lpProcName, _In_ BOOLEAN MappedAsImage );
	void* memset( void* src, int val, unsigned __int64 count );
	void* memcpy( void* dest, const void* src, unsigned __int64 count );
	std::size_t fast_strlen( const char* str );
	void patch( void* address, void* bytes, int byteSize );
	inline void set_console_color( int text = 0x7 /*ConColor::White*/, int background = 0x0 /*ConColor::Black*/ );
	bool does_exists( void* ptr ) noexcept;
	bool IsBadReadPtr( void* p );
	inline bool IsValidPtr( void* p );
	NTSTATUS set_virtual_protection( IN HANDLE process, IN OUT void** baseAddress, IN OUT std::uint32_t* size, IN std::uint32_t newProtection, OUT std::uint32_t* oldProtection );

	HINSTANCE __stdcall get_module_base_wchar( _In_opt_ LPCWSTR lpModuleName );
	HINSTANCE __stdcall get_module_base_ansi( _In_opt_ LPCSTR lpModuleName );

	void* get_interface( const char* dllname, const char* interfacename );
	void* get_interface( HINSTANCE module_handle, const char* interfacename );

	uintptr_t vmt( uintptr_t region, int index );
	uintptr_t vmt1( uintptr_t region, int index );

	uintptr_t find_pattern( const char* module, const char* szSignature, const std::string_view& name = "unknown", bool format = true );
	uintptr_t find_pattern( HINSTANCE module, const char* szSignature, const std::string_view& name = "unknown", bool format = true );

	uintptr_t get_absolute_address( uintptr_t instruction_ptr, int offset, int size = -1 );
}