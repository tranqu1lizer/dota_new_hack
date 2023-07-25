#pragma once

#include "../../definitions.h"

enum SearchPathAdd_t
{
	PATH_ADD_TO_HEAD,		// First path searched
	PATH_ADD_TO_TAIL,		// Last path searched
};

enum FileWarningLevel_t
{
	FILESYSTEM_WARNING = -1,
	FILESYSTEM_WARNING_QUIET = 0,
	FILESYSTEM_WARNING_REPORTUNCLOSED,
	FILESYSTEM_WARNING_REPORTUSAGE,
	FILESYSTEM_WARNING_REPORTALLACCESSES,
	FILESYSTEM_WARNING_REPORTALLACCESSES_READ,
	FILESYSTEM_WARNING_REPORTALLACCESSES_READWRITE,
	FILESYSTEM_WARNING_REPORTALLACCESSES_ASYNC,
};

enum DVDMode_t
{
	DVDMODE_OFF = 0, // not using dvd
	DVDMODE_STRICT = 1, // dvd device only
	DVDMODE_DEV = 2, // dev mode, mutiple devices ok
};

enum KeyValuesPreloadType_t
{
	TYPE_VMT,
	TYPE_SOUNDEMITTER,
	TYPE_SOUNDSCAPE,
	NUM_PRELOAD_TYPES
};

enum FilesystemOpenExFlags_t
{
	FSOPEN_UNBUFFERED = ( 1 << 0 ),
	FSOPEN_FORCE_TRACK_CRC = ( 1 << 1 ),		// This makes it calculate a CRC for the file (if the file came from disk) regardless
	FSOPEN_NEVERINPACK = ( 1 << 2 ),		// 360 only, hint to FS that file is not allowed to be in pack file
};

enum EFileCRCStatus
{
	k_eFileCRCStatus_CantOpenFile,		// We don't have this file.
	k_eFileCRCStatus_GotCRC
};

enum ECacheCRCType
{
	k_eCacheCRCType_SingleFile,
	k_eCacheCRCType_Directory,
	k_eCacheCRCType_Directory_Recursive
};

enum FileSystemSeek_t
{
	FILESYSTEM_SEEK_HEAD = 0,
	FILESYSTEM_SEEK_CURRENT = 1,
	FILESYSTEM_SEEK_TAIL = 2,
};

typedef int FileFindHandle_t;
class FileHandle_t;

class CBaseFileSystem : VClass
{
	DEFINE_INTERFACE( CBaseFileSystem, "filesystem_stdio.dll", "VFileSystem017" );

	FileHandle_t* OpenFile( const char* file_path, const char* options, const char* path_id = nullptr ) {
		return CallVFunc<78, FileHandle_t*>( file_path, options, 0, path_id );
	}

	const char* FindFirst( const char* pWildCard, FileFindHandle_t* pHandle ) {
		return CallVFunc<60, const char*>( pWildCard, pHandle );
	}

	void FindClose( FileFindHandle_t handle ) {
		return CallVFunc<63, void>( handle );
	}

	const char* FindFirstEx( const char* pWildCard, const char* pPathID, FileFindHandle_t* pHandle ) {
		return CallVFunc<64, const char*>( pWildCard, pPathID, pHandle, 0 );
	}

	const char* FindNext( FileFindHandle_t handle ) {
		return CallVFunc<61, const char*>( handle );
	}

	void FindFileAbsoluteList( CUtlVector< CUtlString >* outAbsolutePathNames, const char* pWildCard, const char* pPathID ) {
		return CallVFunc<65, void>( outAbsolutePathNames, pWildCard, pPathID );
	}

	void Close( FileHandle_t* handle ) {
		return CallVFunc<14, void>( handle );
	}

	unsigned int GetFileSize( FileHandle_t* handle ) {
		return CallVFunc<18, unsigned int>( handle );
	}

	unsigned int Write( void const* input, int size, FileHandle_t* file ) {
		return CallVFunc<12, unsigned int>( input, size, file );
	}

	char* ReadLine( char* buffer, int size, FileHandle_t* handle ) {
		return CallVFunc<56, char*>( buffer, size, handle );
	}

	int ReadEx( char* buffer, int size_dest, int size, FileHandle_t* handle ) {
		return CallVFunc<79, int>( buffer, size_dest, size, handle );
	}

	void AddSearchPath( const char* path_str, const char* path_id, SearchPathAdd_t type_of_path, int priority ) {
		return CallVFunc<31, void>( path_str, path_id, type_of_path, priority );
	}

	void RemoveSearchPath( const char* path_str, const char* path_id ) {
		return CallVFunc<32, void>( path_str, path_id );
	}
};