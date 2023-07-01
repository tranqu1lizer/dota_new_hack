#include "util.hpp"
#include "../sdk_game/valve/bitstream.hpp"
#include "address_wrapper.hxx"
#include "../sdk_game/CSource2Client.hpp"

void util::patch( void* address, const void* bytes, int byteSize )
{
	DWORD NewProtection;
	VirtualProtect( address, byteSize, PAGE_EXECUTE_READWRITE, &NewProtection );
	util::memcpy( address, bytes, byteSize );
	VirtualProtect( address, byteSize, NewProtection, &NewProtection );
}

void util::patch( void* address, const void* bytes ) {
	util::patch( address, bytes, util::fast_strlen( (const char*)bytes ) );
}
char* util::fast_strcat( char* dest, const char* src )
{
	char* rdest = dest;

	while ( *dest )
		dest++;
	while ( *dest++ = *src++ )
		;
	return rdest;
}
void util::allocate_console( ) {
	AllocConsole( );

	FILE* fDummy;
	freopen_s( &fDummy, "CONIN$", "r", stdin );
	freopen_s( &fDummy, "CONOUT$", "w", stderr );
	freopen_s( &fDummy, "CONOUT$", "w", stdout );

	HANDLE hStdout = CreateFile( L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
								 NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	HANDLE hStdin = CreateFile( L"CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
								NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	SetStdHandle( STD_OUTPUT_HANDLE, hStdout );
	SetStdHandle( STD_ERROR_HANDLE, hStdout );
	SetStdHandle( STD_INPUT_HANDLE, hStdin );

	SetConsoleOutputCP( 1251 );

	std::wclog.clear( );
	std::clog.clear( );
	std::wcout.clear( );
	std::cout.clear( );
	std::wcerr.clear( );
	std::cerr.clear( );
	std::wcin.clear( );
	std::cin.clear( );
}

void* util::memset( void* src, int val, unsigned __int64 count )
{
	__stosb( (unsigned char*)( ( unsigned long long )( volatile char* )src ), val, count );
	return src;
}

void* util::find_game_system( const char* name ) {
	static IGameSystemFactory* factory_it = nullptr;
	if ( !factory_it ) {
		factory_it = *AddressWrapper{ util::find_pattern( "client.dll", "E8 ? ? ? ? 84 C0 74 D3 48 8D 0D" ) }
			.get_address_from_instruction_ptr( 1 )
			.get_offset( 0xE )
			.get_address_from_instruction_ptr<IGameSystemFactory**>( 3 );
	}

	while ( factory_it && factory_it->m_name ) {
		if ( !util::fast_strcmp( (char*)factory_it->m_name, (char*)name ) ) {
			const auto res = factory_it->game_system( );
			return res ? res : factory_it->get_game_system();
		}

		factory_it = factory_it->m_next_factory;
	}

	return nullptr;
}

NTSTATUS util::set_virtual_protection( IN HANDLE process, IN OUT void** baseAddress, IN OUT std::uint32_t* size, IN std::uint32_t newProtection, OUT std::uint32_t* oldProtection ) {
	typedef NTSTATUS( WINAPI* tNtPVM )( IN HANDLE ProcessHandle, IN OUT void** BaseAddress, IN OUT std::uint32_t* NumberOfBytesToProtect, IN std::uint32_t NewAccessProtection, OUT std::uint32_t* OldAccessProtection );
	static tNtPVM zwProtectVirtualMemory = nullptr;

	if ( !zwProtectVirtualMemory ) {
		const auto ntdll = util::get_module_base_wchar( L"ntdll.dll" );
		if ( !ntdll ) return STATUS_DLL_NOT_FOUND;
		zwProtectVirtualMemory = reinterpret_cast<tNtPVM>( util::find_export_address( ntdll, "NtProtectVirtualMemory" ) );
		if ( !zwProtectVirtualMemory ) return STATUS_ENTRYPOINT_NOT_FOUND;
	}

	return zwProtectVirtualMemory( process, baseAddress, size, newProtection, oldProtection );
}

int util::memcmp( const void* buf1,
		   const void* buf2,
		   size_t count )
{
	if ( !count )
		return( 0 );

	while ( --count && *(char*)buf1 == *(char*)buf2 ) {
		buf1 = (char*)buf1 + 1;
		buf2 = (char*)buf2 + 1;
	}

	return( *( (unsigned char*)buf1 ) - *( (unsigned char*)buf2 ) );
}

void* util::memcpy( void* dest, const void* src, unsigned __int64 count ) {
	char* char_dest = (char*)dest;
	char* char_src = (char*)src;
	if ( ( char_dest <= char_src ) || ( char_dest >= ( char_src + count ) ) )
	{
		while ( count > 0 )
		{
			*char_dest = *char_src;
			char_dest++;
			char_src++;
			count--;
		}
	}
	else
	{
		char_dest = (char*)dest + count - 1;
		char_src = (char*)src + count - 1;
		while ( count > 0 )
		{
			*char_dest = *char_src;
			char_dest--;
			char_src--;
			count--;
		}
	}
	return dest;
}

bool util::exists( void* ptr ) noexcept {
	return ptr && *reinterpret_cast<std::uintptr_t*>( ptr ) != 0xdddddddddddddddd && !IsBadReadPtr( ptr );
}

bool util::IsBadReadPtr( void* p )
{
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	if ( ::VirtualQuery( p, &mbi, sizeof( mbi ) ) )
	{
		DWORD mask = ( PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY );
		bool b = !( mbi.Protect & mask );

		if ( mbi.Protect & ( PAGE_GUARD | PAGE_NOACCESS ) ) b = true;

		return b;
	}
	return true;
}

inline bool util::IsValidPtr( void* p ) {
	if ( !p )
		return false;
	MEMORY_BASIC_INFORMATION mbi;
	util::memset( &mbi, 0, sizeof( mbi ) );
	if ( !VirtualQuery( (void*)p, &mbi, sizeof( mbi ) ) )
		return false;
	if ( !( mbi.State & MEM_COMMIT ) )
		return false;
	if ( !( mbi.Protect & ( PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY |
		 PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY ) ) )
		return false;
	if ( mbi.Protect & ( PAGE_GUARD | PAGE_NOACCESS ) )
		return false;
	return true;
}

void util::clear_console( ) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	SMALL_RECT scrollRect;
	COORD scrollTarget;
	CHAR_INFO fill;

	const auto hconsole = GetStdHandle( STD_OUTPUT_HANDLE );

	if ( !GetConsoleScreenBufferInfo( hconsole, &csbi ) )
	{
		return;
	}

	scrollRect.Left = 0;
	scrollRect.Top = 0;
	scrollRect.Right = csbi.dwSize.X;
	scrollRect.Bottom = csbi.dwSize.Y;
	scrollTarget.X = 0;
	scrollTarget.Y = (SHORT)( 0 - csbi.dwSize.Y );
	fill.Char.UnicodeChar = TEXT( ' ' );
	fill.Attributes = csbi.wAttributes;

	ScrollConsoleScreenBuffer( hconsole, &scrollRect, NULL, scrollTarget, &fill );

	csbi.dwCursorPosition.X = 0;
	csbi.dwCursorPosition.Y = 0;

	SetConsoleCursorPosition( hconsole, csbi.dwCursorPosition );
}

char* util::fast_strstr( const char* haystack, const char* needle )
{
	if ( !*needle )
		return (char*)haystack;

	const char needle_first = *needle;

	haystack = strchr( haystack, needle_first );
	if ( !haystack )
		return NULL;

	const char* i_haystack = haystack + 1
		, * i_needle = needle + 1;

	unsigned int sums_diff = *haystack;
	bool identical = true;

	while ( *i_haystack && *i_needle ) {
		sums_diff += *i_haystack;
		sums_diff -= *i_needle;
		identical &= *i_haystack++ == *i_needle++;
	}

	if ( *i_needle )
		return NULL;
	else if ( identical )
		return (char*)haystack;

	size_t needle_len = i_needle - needle;
	const auto needle_len_1 = needle_len - 1;

	const char* sub_start;
	for ( sub_start = haystack; *i_haystack; i_haystack++ ) {
		sums_diff -= *sub_start++;
		sums_diff += *i_haystack;

		if (
			sums_diff == 0
			&& needle_first == *sub_start
			&& memcmp( sub_start, needle, needle_len_1 ) == 0
			)
			return (char*)sub_start;
	}

	return NULL;
}

int util::fast_strcmp( char* s1, char* s2 )
{
	while ( *s1 == *s2++ )
		if ( *s1++ == 0 )
			return ( 0 );
	return ( *(const unsigned char*)s1 - *(const unsigned char*)( s2 - 1 ) );
}

FARPROC __stdcall util::find_export_address( HMODULE hModule, const char* ImportName )
{
	PVOID ProcedureAddress = NULL;
	PVOID ImageBase = (PVOID)hModule;
	if ( ImageBase )
	{
		PIMAGE_DOS_HEADER pIDH = (PIMAGE_DOS_HEADER)ImageBase;
		if ( pIDH->e_magic == IMAGE_DOS_SIGNATURE )
		{
			PIMAGE_NT_HEADERS pINH = (PIMAGE_NT_HEADERS)( (PUCHAR)ImageBase + pIDH->e_lfanew );
			if ( pINH->Signature == IMAGE_NT_SIGNATURE )
			{
				PIMAGE_DATA_DIRECTORY pIDD = &pINH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
				if ( pIDD->VirtualAddress && pIDD->Size )
				{
					PIMAGE_EXPORT_DIRECTORY pIED = (PIMAGE_EXPORT_DIRECTORY)( (PUCHAR)ImageBase + pIDD->VirtualAddress );

					PULONG Names = (PULONG)( (PUCHAR)ImageBase + pIED->AddressOfNames );
					PULONG Functions = (PULONG)( (PUCHAR)ImageBase + pIED->AddressOfFunctions );
					PUSHORT NameOrdinals = (PUSHORT)( (PUCHAR)ImageBase + pIED->AddressOfNameOrdinals );

					if ( (ULONG_PTR)ImportName <= 0xFFFF )
					{
						ProcedureAddress = (PUCHAR)ImageBase + Functions[(ULONG_PTR)ImportName];
					}
					else
					{
						for ( ULONG i = 0; i < pIED->NumberOfNames; i++ )
						{
							if ( strcmp( (PCHAR)ImageBase + Names[i], ImportName ) == 0 )
							{
								ProcedureAddress = (PUCHAR)ImageBase + Functions[NameOrdinals[i]];
								break;
							}
						}
					}
				}
			}
		}
	}
	return (FARPROC)ProcedureAddress;
}

std::string util::utf8_encode( const std::wstring& wstr )
{
	if ( wstr.empty( ) ) return std::string( );
	int size_needed = WideCharToMultiByte( CP_UTF8, 0, &wstr[0], (int)wstr.size( ), NULL, 0, NULL, NULL );
	std::string strTo( size_needed, 0 );
	WideCharToMultiByte( CP_UTF8, 0, &wstr[0], (int)wstr.size( ), &strTo[0], size_needed, NULL, NULL );
	return strTo;
}

// Convert an UTF8 string to a wide Unicode String
std::wstring util::utf8_decode( const std::string& str )
{
	if ( str.empty( ) ) return std::wstring( );
	int size_needed = MultiByteToWideChar( CP_UTF8, 0, &str[0], (int)str.size( ), NULL, 0 );
	std::wstring wstrTo( size_needed, 0 );
	MultiByteToWideChar( CP_UTF8, 0, &str[0], (int)str.size( ), &wstrTo[0], size_needed );
	return wstrTo;
}

HINSTANCE util::get_module_base_wchar( const wchar_t* lpModuleName )
{
	PPEB64 peb = reinterpret_cast<PPEB64>( __readgsqword( 0x60 ) ); //x64
	auto ldr_data = reinterpret_cast<PPEB_LDR_DATA>( peb->Ldr );

	for ( PLIST_ENTRY list_entry = ldr_data->InLoadOrderModuleList.Flink; list_entry != &ldr_data->InLoadOrderModuleList; list_entry = list_entry->Flink )
	{
		if ( PLDR_DATA_TABLE_ENTRY ldr_entry = CONTAINING_RECORD( list_entry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks ); ldr_entry )
		{
			if ( _wcsicmp( lpModuleName, ldr_entry->BaseDllName.Buffer ) == 0 )
			{
				return static_cast<HINSTANCE>( ldr_entry->DllBase );
			}
		}
	}

	return 0;
}

HINSTANCE __stdcall util::get_module_base_ansi( const char* lpModuleName )
{
	if ( !lpModuleName ) return NULL;

	const auto wide_str = util::utf8_decode( lpModuleName );
	const auto ret = util::get_module_base_wchar( wide_str.c_str( ) );
	return ret;
}

constexpr int UBV_COUNT[] = { 0, 4, 8, 28 };

constexpr unsigned long long MASKS[] = {
	   0x0L,               0x1L,                0x3L,                0x7L,
	   0xfL,               0x1fL,               0x3fL,               0x7fL,
	   0xffL,              0x1ffL,              0x3ffL,              0x7ffL,
	   0xfffL,             0x1fffL,             0x3fffL,             0x7fffL,
	   0xffffL,            0x1ffffL,            0x3ffffL,            0x7ffffL,
	   0xfffffL,           0x1fffffL,           0x3fffffL,           0x7fffffL,
	   0xffffffL,          0x1ffffffL,          0x3ffffffL,          0x7ffffffL,
	   0xfffffffL,         0x1fffffffL,         0x3fffffffL,         0x7fffffffL,
	   0xffffffffL,        0x1ffffffffL,        0x3ffffffffL,        0x7ffffffffL,
	   0xfffffffffL,       0x1fffffffffL,       0x3fffffffffL,       0x7fffffffffL,
	   0xffffffffffL,      0x1ffffffffffL,      0x3ffffffffffL,      0x7ffffffffffL,
	   0xfffffffffffL,     0x1fffffffffffL,     0x3fffffffffffL,     0x7fffffffffffL,
	   0xffffffffffffL,    0x1ffffffffffffL,    0x3ffffffffffffL,    0x7ffffffffffffL,
	   0xfffffffffffffL,   0x1fffffffffffffL,   0x3fffffffffffffL,   0x7fffffffffffffL,
	   0xffffffffffffffL,  0x1ffffffffffffffL,  0x3ffffffffffffffL,  0x7ffffffffffffffL,
	   0xfffffffffffffffL, 0x1fffffffffffffffL, 0x3fffffffffffffffL, 0x7fffffffffffffffL,
	   0xffffffffffffffffL
};

void* util::get_interface( const char* dllname, const char* interfacename ) {
	const auto module_handle = util::get_module_base_ansi( dllname );
	if ( module_handle ) {
		const auto export_address = util::find_export_address( module_handle, "CreateInterface" );
		return reinterpret_cast<void* ( * )( const char* pName, int* pReturnCode )>( export_address )( interfacename, nullptr );
	}
	return nullptr;
}

void* util::get_interface( HINSTANCE module_handle, const char* interfacename ) {
	if ( module_handle ) {
		const auto export_address = util::find_export_address( module_handle, "CreateInterface" );
		return reinterpret_cast<void* ( * )( const char* pName, int* pReturnCode )>( export_address )( interfacename, nullptr );
	}
	return nullptr;
}

uintptr_t util::vmt( uintptr_t region, int index ) {
	const auto vfunc = *reinterpret_cast<uintptr_t*>( *reinterpret_cast<uintptr_t*>( region ) + index * sizeof( std::ptrdiff_t ) );
	return IsValidPtr( (void*)vfunc ) ? vfunc : 0;
}

uintptr_t util::vmt1( uintptr_t region, int index ) {
	return *reinterpret_cast<uintptr_t*>( region + index * sizeof( std::ptrdiff_t ) );
}

std::size_t util::fast_strlen( const char* str )
{
	const char* s;

	for ( s = str; *s; ++s )
		;
	return ( s - str );
}

inline void util::set_console_color( int text, int background ) {
	/*
	enum class ConColor {
		Black,
		Blue,
		Green,
		Teal,
		Red,
		Lily,
		Yellow,
		White,
		Grey,
		LightBlue,
		LightGreen,
		LightTeal,
		LightRed,
		LightLily,
		LightYellow,
		BrightWhite
	};
	*/
	static void* hStdOut = nullptr;
	if ( !hStdOut ) hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
	SetConsoleTextAttribute( hStdOut, (WORD)( ( (WORD)background << 4 ) | (WORD)text ) );
};

std::string ParseCombo( const std::string& combo )
{
	std::string pattern;
	pattern.reserve( ( combo.size( ) + 1 ) / 3 );

	for ( unsigned int i = 0; i < combo.size( ); i++ )
	{
		if ( combo[i] == ' ' )
		{
			continue;
		}

		else if ( combo[i] == '?' )
		{
			pattern += '\xCC';
			i += 1;
		}
		else
		{
			const auto byte = (char)strtol( &combo[i], 0, 16 );
			pattern += byte;
			i += 2;
		}
	}
	return pattern;
}

FORCEINLINE std::array<size_t, 256> FillShiftTable( const char* pattern, const uint8_t wildcard ) {
	std::array<size_t, 256> bad_char_skip = {};
	size_t idx = 0;
	const size_t last = util::fast_strlen( pattern ) - 1;

	// Get last wildcard position
	for ( idx = last; idx > 0 && (uint8_t)pattern[idx] != wildcard; --idx );
	size_t diff = last - idx;
	if ( diff == 0 )
		diff = 1;

	// Prepare shift table
	for ( idx = 0; idx < 256; ++idx )
		bad_char_skip[idx] = diff;
	for ( idx = last - diff; idx < last; ++idx )
		bad_char_skip[(uint8_t)pattern[idx]] = last - idx;
	return bad_char_skip;
}

void* find_pattern_internal( uintptr_t begin, uintptr_t end, const char* pattern )
{
	auto scanPos = (uint8_t*)begin;
	const auto scanEnd = (uint8_t*)end;
	const size_t last = util::fast_strlen( pattern ) - 1;
	const auto bad_char_skip = FillShiftTable( pattern, 0xCC );

	for ( ; scanPos <= scanEnd; scanPos += bad_char_skip[scanPos[last]] )
		for ( size_t idx = last; idx >= 0; --idx ) {
			const uint8_t elem = pattern[idx];
			if ( elem != 0xCC && elem != scanPos[idx] )
				break;
			if ( idx == 0 )
				return scanPos;
		}

	return nullptr;
}

std::uintptr_t util::find_pattern( const char* smodule, const char* pattern, const std::string_view& name, bool format )
{
	const auto is_dbg = name.empty( );
	const auto start = std::chrono::high_resolution_clock::now( );

	const auto begin = (uintptr_t)util::get_module_base_ansi( smodule );
	const auto dwSizeOfImage = PIMAGE_NT_HEADERS( (uint8_t*)( begin + PIMAGE_DOS_HEADER( begin )->e_lfanew ) )->OptionalHeader.SizeOfImage;
	const auto end = begin + dwSizeOfImage;

	if ( const auto result = reinterpret_cast<std::uintptr_t>( find_pattern_internal( begin, end, format ? ParseCombo( pattern ).c_str( ) : pattern ) ); result ) {
		if ( !is_dbg ) {
			const auto duration = duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now( ) - start );

			spdlog::debug( "{}: ", name ); util::set_console_color( 0xA );
			std::cout << "0x" << std::hex << result; util::set_console_color( );
			std::cout << " (" << std::dec << duration.count( ) << "ms)\n";
		}
		return result;
	}
	if ( is_dbg ) spdlog::debug( "{} not found :(\n", name );
	return NULL;
}

std::uintptr_t util::find_pattern( HINSTANCE pmodule, const char* pattern, const std::string_view& name, bool format )
{
	const auto is_dbg = name.empty( );
	const auto start = std::chrono::high_resolution_clock::now( );

	const auto begin = (uintptr_t)pmodule;
	const auto size_of_image = PIMAGE_NT_HEADERS( (uint8_t*)( begin + PIMAGE_DOS_HEADER( begin )->e_lfanew ) )->OptionalHeader.SizeOfImage;
	const auto end = begin + size_of_image;

	if ( const auto result = reinterpret_cast<std::uintptr_t>( find_pattern_internal( begin, end, format ? ParseCombo( pattern ).c_str( ) : pattern ) ); result ) {
		if ( !is_dbg ) {
			const auto duration = duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now( ) - start );

			spdlog::debug( "{}: ", name ); util::set_console_color( 0xA );
			std::cout << "0x" << std::hex << result; util::set_console_color( );
			std::cout << " (" << std::dec << duration.count( ) << "ms)\n";
		}
		return result;
	}
	if ( is_dbg ) spdlog::debug("{} not found :(\n", name );
	return NULL;
}

uintptr_t util::get_absolute_address( uintptr_t instruction_ptr, int offset, int size )
{
	if ( !instruction_ptr ) return 0;
	if ( size == -1 )
		size = instruction_ptr + sizeof( uint32_t );
	const auto result = instruction_ptr + *reinterpret_cast<int32_t*>( instruction_ptr + offset ) + size;
	return IsValidPtr( (void*)result ) ? result : 0;
}