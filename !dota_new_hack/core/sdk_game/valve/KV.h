#pragma once

#include "CBaseFileSystem.hpp"

class KeyValues {
public:
	static auto Make( const char* name ) {
		const void* kv = CMemAlloc::GetInstance( )->allocate( 0x14 );
		auto ctor = (some_function)util::find_export_address( global::tier0, "??0KeyValues@@QEAA@PEBD@Z" );
		ctor( kv, name );
		return ( KeyValues* )kv;
	}

	bool LoadFromFile( const char* path, char* const pathId = "GAME" ) {
		auto func = (some_function)util::find_export_address( global::tier0, "?LoadFromFile@KeyValues@@QEAA_NPEAVIFileSystem@@PEBD1P6A_N1PEAX@Z21@Z" );
		return func.call<bool>( this, &CBaseFileSystem::get( ), path, pathId, nullptr, nullptr, nullptr );
	}

	KeyValues* GetFirstSubKey( ) {
		auto func = (some_function)util::find_export_address( global::tier0, "?GetFirstSubKey@KeyValues@@QEBAPEAV1@XZ" );
		return func.call<KeyValues*>( this );
	}

	KeyValues* GetNextKey( ) {
		auto func = (some_function)util::find_export_address( global::tier0, "?GetNextKey@KeyValues@@QEBAPEAV1@XZ" );
		return func.call<KeyValues*>( this );
	}

	const char* GetName( ) {
		auto fn = (some_function)util::find_export_address( global::tier0, "?GetName@KeyValues@@QEBAPEBDXZ" );
		return fn.call<const char*>( this );
	}
	
	const char* GetString( ) {
		auto fn = (some_function)util::find_export_address( global::tier0, "?GetString@KeyValues@@QEAAPEBDPEBD0PEAD_K@Z" );
		return fn.call<const char*>( this );
	}
};