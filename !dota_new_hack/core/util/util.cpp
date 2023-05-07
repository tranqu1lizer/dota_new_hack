#include "util.hpp"

PVOID WINAPI ImageDirectoryEntryToDataInternal( PVOID Base, BOOLEAN MappedAsImage, ULONG* Size, DWORD SizeOfHeaders, IMAGE_DATA_DIRECTORY* DataDirectory, IMAGE_FILE_HEADER* ImageFileHeader, void* ImageOptionalHeader )
{
	*(ULONG*)Size = NULL;

	if ( !DataDirectory->VirtualAddress || !DataDirectory->Size || !SizeOfHeaders )
		return nullptr;

	*(ULONG*)Size = DataDirectory->Size;
	if ( MappedAsImage || DataDirectory->VirtualAddress < SizeOfHeaders )
		return (char*)Base + DataDirectory->VirtualAddress;

	WORD SizeOfOptionalHeader = ImageFileHeader->SizeOfOptionalHeader;
	WORD NumberOfSections = ImageFileHeader->NumberOfSections;
	if ( !NumberOfSections || !SizeOfOptionalHeader )
		return nullptr;

	IMAGE_SECTION_HEADER* pSectionHeaders = (IMAGE_SECTION_HEADER*)( (BYTE*)ImageOptionalHeader + SizeOfOptionalHeader );
	for ( DWORD i = 0; i < NumberOfSections; i++ )
	{
		IMAGE_SECTION_HEADER* pSectionHeader = &pSectionHeaders[i];
		if ( ( DataDirectory->VirtualAddress >= pSectionHeader->VirtualAddress ) &&
			 ( DataDirectory->VirtualAddress < ( pSectionHeader->SizeOfRawData + pSectionHeader->VirtualAddress ) ) )
		{
			return (char*)Base + ( DataDirectory->VirtualAddress - pSectionHeader->VirtualAddress ) + pSectionHeader->PointerToRawData;
		}
	}
	return nullptr;
}

PVOID WINAPI ImageDirectoryEntryToData32( PVOID Base, BOOLEAN MappedAsImage, USHORT DirectoryEntry, ULONG* Size, IMAGE_FILE_HEADER* ImageFileHeader, IMAGE_OPTIONAL_HEADER32* ImageOptionalHeader )
{
	*(ULONG*)Size = NULL;

	if ( DirectoryEntry >= ImageOptionalHeader->NumberOfRvaAndSizes )
		return nullptr;

	IMAGE_DATA_DIRECTORY* DataDirectory = &ImageOptionalHeader->DataDirectory[DirectoryEntry];
	if ( !DataDirectory->VirtualAddress || !DataDirectory->Size )
		return nullptr;

	return ImageDirectoryEntryToDataInternal( Base,
											  MappedAsImage,
											  Size,
											  ImageOptionalHeader->SizeOfHeaders,
											  DataDirectory,
											  ImageFileHeader,
											  ImageOptionalHeader );
}

PVOID WINAPI ImageDirectoryEntryToData64( PVOID Base, BOOLEAN MappedAsImage, USHORT DirectoryEntry, ULONG* Size, IMAGE_FILE_HEADER* ImageFileHeader, IMAGE_OPTIONAL_HEADER64* ImageOptionalHeader )
{
	*(ULONG*)Size = NULL;

	if ( DirectoryEntry >= ImageOptionalHeader->NumberOfRvaAndSizes )
		return nullptr;

	IMAGE_DATA_DIRECTORY* DataDirectory = &ImageOptionalHeader->DataDirectory[DirectoryEntry];
	if ( !DataDirectory->VirtualAddress || !DataDirectory->Size )
		return nullptr;

	return ImageDirectoryEntryToDataInternal( Base,
											  MappedAsImage,
											  Size,
											  ImageOptionalHeader->SizeOfHeaders,
											  DataDirectory,
											  ImageFileHeader,
											  ImageOptionalHeader );
}

PVOID WINAPI ImageDirectoryEntryToDataRom( PVOID Base, WORD HeaderMagic, USHORT DirectoryEntry, ULONG* Size, IMAGE_FILE_HEADER* ImageFileHeader, IMAGE_ROM_OPTIONAL_HEADER* ImageRomHeaders )
{
	*(ULONG*)Size = NULL;

	if ( ImageFileHeader->NumberOfSections <= 0u || !ImageFileHeader->SizeOfOptionalHeader )
		return nullptr;

	IMAGE_SECTION_HEADER* pSectionHeader = (IMAGE_SECTION_HEADER*)( (BYTE*)ImageRomHeaders + ImageFileHeader->SizeOfOptionalHeader );

	WORD j = 0;
	for ( ; j < ImageFileHeader->NumberOfSections; j++, pSectionHeader++ )
	{
		if ( DirectoryEntry == 3 && _stricmp( (char*)pSectionHeader->Name, ".pdata" ) == NULL )
			break;
		if ( DirectoryEntry == 6 && _stricmp( (char*)pSectionHeader->Name, ".rdata" ) == NULL )
		{
			*(ULONG*)Size = NULL;
			for ( BYTE* i = (BYTE*)Base + pSectionHeader->PointerToRawData + 0xC; *(DWORD*)i; i += 0x1C )
				*Size += 0x1C;
			break;
		}
	}
	if ( j >= ImageFileHeader->NumberOfSections )
		return nullptr;

	return (char*)Base + pSectionHeader->PointerToRawData;
}

PVOID WINAPI ImageDirectoryEntryToDataEx( PVOID Base, BOOLEAN MappedAsImage, USHORT DirectoryEntry, ULONG* Size )
{
	*(ULONG*)Size = NULL;

	IMAGE_DOS_HEADER* pDosHeader = (IMAGE_DOS_HEADER*)Base;
	if ( !pDosHeader )
		return nullptr;

	IMAGE_FILE_HEADER* ImageFileHeader = nullptr;
	IMAGE_OPTIONAL_HEADER* ImageOptionalHeader = nullptr;

	LONG NtHeaderFileOffset = pDosHeader->e_lfanew;
	IMAGE_NT_HEADERS* ImageNtHeader = (PIMAGE_NT_HEADERS)( (LPBYTE)pDosHeader + NtHeaderFileOffset );

	if ( pDosHeader->e_magic == IMAGE_DOS_SIGNATURE
		 && NtHeaderFileOffset > 0
		 && NtHeaderFileOffset < 0x10000000u
		 && ImageNtHeader->Signature == IMAGE_NT_SIGNATURE )
	{
		ImageFileHeader = &ImageNtHeader->FileHeader;
		ImageOptionalHeader = &ImageNtHeader->OptionalHeader;
	}
	else
	{
		ImageFileHeader = (IMAGE_FILE_HEADER*)Base;
		ImageOptionalHeader = (IMAGE_OPTIONAL_HEADER*)( (BYTE*)Base + 0x14 );
	}
	switch ( ImageOptionalHeader->Magic )
	{
	case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
		return ImageDirectoryEntryToData32(
			Base,
			MappedAsImage,
			DirectoryEntry,
			Size,
			ImageFileHeader,
			(IMAGE_OPTIONAL_HEADER32*)ImageOptionalHeader );
	case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
		return ImageDirectoryEntryToData64(
			Base,
			MappedAsImage,
			DirectoryEntry,
			Size,
			ImageFileHeader,
			(IMAGE_OPTIONAL_HEADER64*)ImageOptionalHeader );
	case IMAGE_ROM_OPTIONAL_HDR_MAGIC:
		return ImageDirectoryEntryToDataRom(
			Base,
			IMAGE_ROM_OPTIONAL_HDR_MAGIC,
			DirectoryEntry,
			Size,
			ImageFileHeader,
			(IMAGE_ROM_OPTIONAL_HEADER*)ImageOptionalHeader );
	}
	return nullptr;
}

void util::patch( void* address, void* bytes, int byteSize )
{
	DWORD NewProtection;
	VirtualProtect( address, byteSize, PAGE_EXECUTE_READWRITE, &NewProtection );
	util::memcpy( address, bytes, byteSize );
	VirtualProtect( address, byteSize, NewProtection, &NewProtection );
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

NTSTATUS util::set_virtual_protection( IN HANDLE process, IN OUT void** baseAddress, IN OUT std::uint32_t* size, IN std::uint32_t newProtection, OUT std::uint32_t* oldProtection ) {
	typedef NTSTATUS( WINAPI* tNtPVM )( IN HANDLE ProcessHandle, IN OUT void** BaseAddress, IN OUT std::uint32_t* NumberOfBytesToProtect, IN std::uint32_t NewAccessProtection, OUT std::uint32_t* OldAccessProtection );
	static tNtPVM zwProtectVirtualMemory = nullptr;

	if ( !zwProtectVirtualMemory ) {
		const auto ntdll = util::get_module_base_wchar( L"ntdll.dll" );
		if ( !ntdll ) return STATUS_DLL_NOT_FOUND;
		zwProtectVirtualMemory = reinterpret_cast<tNtPVM>( util::find_export_address( ntdll, "NtProtectVirtualMemory", true ) );
		if ( !zwProtectVirtualMemory ) return STATUS_ENTRYPOINT_NOT_FOUND;
	}

	return zwProtectVirtualMemory( process, baseAddress, size, newProtection, oldProtection );
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

bool util::does_exists( void* ptr ) noexcept {
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

IMAGE_SECTION_HEADER* WINAPI ImageRvaToSection( PIMAGE_NT_HEADERS NtHeaders, PVOID Base, ULONG Rva )
{
	if ( !NtHeaders )
		return nullptr;

	DWORD dwNumberOfSections = NtHeaders->FileHeader.NumberOfSections;
	if ( !dwNumberOfSections )
		return nullptr;

	WORD SizeOfOptionalHeader = NtHeaders->FileHeader.SizeOfOptionalHeader;
	IMAGE_SECTION_HEADER* pSectionHeaders = (IMAGE_SECTION_HEADER*)( (BYTE*)&NtHeaders->OptionalHeader + SizeOfOptionalHeader );
	for ( DWORD i = 0; i < dwNumberOfSections; i++ )
	{
		DWORD VirtualAddress = pSectionHeaders[i].VirtualAddress;
		DWORD SizeOfRawData = pSectionHeaders[i].SizeOfRawData;
		if ( ( Rva >= VirtualAddress ) && ( Rva < ( SizeOfRawData + VirtualAddress ) ) )
			return &pSectionHeaders[i];
	}
	return nullptr;
}

PVOID WINAPI ImageRvaToVa( PIMAGE_NT_HEADERS NtHeaders, void* Base, DWORD Rva )
{
	IMAGE_SECTION_HEADER* ResultSection = nullptr;

	ResultSection = ImageRvaToSection( NtHeaders, (PVOID)Base, Rva );
	if ( !ResultSection )
		return nullptr;

	return (char*)Base + ( Rva - ResultSection->VirtualAddress ) + ResultSection->PointerToRawData;
}

FARPROC __stdcall util::find_export_address( _In_ HMODULE hModule, _In_ LPCSTR lpProcName, _In_ BOOLEAN MappedAsImage )
{
	if ( lpProcName == NULL )
		return nullptr;



	unsigned short ProcOrdinal = 0xFFFF;
	if ( (ULONG_PTR)lpProcName < 0xFFFF )
		ProcOrdinal = (ULONG_PTR)lpProcName & 0xFFFF;
	else
	{
		//in case of "#123" resolve the ordinal to 123
		if ( lpProcName[0] == '#' )
		{
			DWORD OrdinalFromString = atoi( lpProcName + 1 );
			if ( OrdinalFromString < 0xFFFF &&
				 OrdinalFromString != 0 )
			{
				ProcOrdinal = OrdinalFromString & 0xFFFF;
				lpProcName = (LPCSTR)( ProcOrdinal );
			}
		}
	}
	IMAGE_DOS_HEADER* DosHeader = (IMAGE_DOS_HEADER*)hModule;
	if ( !DosHeader || DosHeader->e_magic != IMAGE_DOS_SIGNATURE )
		return nullptr;

	//only OptionalHeader is different between 64bit and 32bit so try not to touch it!
	IMAGE_NT_HEADERS* NtHeader = (IMAGE_NT_HEADERS*)( (DWORD_PTR)DosHeader + DosHeader->e_lfanew );
	if ( NtHeader->Signature != IMAGE_NT_SIGNATURE )
		return nullptr;

	ULONG ExportDirectorySize = NULL;
	IMAGE_EXPORT_DIRECTORY* ExportDirectory = (IMAGE_EXPORT_DIRECTORY*)ImageDirectoryEntryToDataEx( DosHeader, MappedAsImage, IMAGE_DIRECTORY_ENTRY_EXPORT, &ExportDirectorySize );
	if ( !ExportDirectory || !ExportDirectorySize )
		return nullptr;

	//check if any export functions are present
	if ( !ExportDirectory->NumberOfFunctions )
		return nullptr;

	//from BlackBone
	//https://github.com/DarthTon/Blackbone/blob/3dc33d815011b83855af607013d34c836b9d0877/src/BlackBone/Process/ProcessModules.cpp#L266
	// Fix invalid directory size
	if ( ExportDirectorySize <= sizeof( IMAGE_EXPORT_DIRECTORY ) )
	{
		// New size should take care of max number of present names (max name length is assumed to be 255 chars)
		ExportDirectorySize = static_cast<DWORD>( ExportDirectory->AddressOfNameOrdinals - (DWORD)( (BYTE*)(ExportDirectory)-(BYTE*)( DosHeader ) )
												  + max( ExportDirectory->NumberOfFunctions, ExportDirectory->NumberOfNames ) * 255 );
	}

	DWORD AddressOfNamesRVA = ExportDirectory->AddressOfNames;
	DWORD AddressOfFunctionsRVA = ExportDirectory->AddressOfFunctions;
	DWORD AddressOfNameOrdinalsRVA = ExportDirectory->AddressOfNameOrdinals;

	DWORD* ExportNames = (DWORD*)( MappedAsImage ? ( (BYTE*)DosHeader + AddressOfNamesRVA ) : ImageRvaToVa( NtHeader, DosHeader, AddressOfNamesRVA ) );
	DWORD* Functions = (DWORD*)( MappedAsImage ? ( (BYTE*)DosHeader + AddressOfFunctionsRVA ) : ImageRvaToVa( NtHeader, DosHeader, AddressOfFunctionsRVA ) );
	WORD* Ordinals = (WORD*)( MappedAsImage ? ( (BYTE*)DosHeader + AddressOfNameOrdinalsRVA ) : ImageRvaToVa( NtHeader, DosHeader, AddressOfNameOrdinalsRVA ) );

	for ( DWORD i = 0; i < ExportDirectory->NumberOfFunctions; i++ )
	{
		unsigned short OrdinalIndex = Ordinals[i];

		DWORD ExportFncOffset = Functions[OrdinalIndex];
		if ( !ExportFncOffset )
			continue;

		char* ProcNamePtr = (char*)( MappedAsImage ? ( (char*)DosHeader + ExportNames[i] ) : ImageRvaToVa( NtHeader, DosHeader, ExportNames[i] ) );
		BYTE* ExportFnc = (BYTE*)( MappedAsImage ? ( (BYTE*)DosHeader + ExportFncOffset ) : ImageRvaToVa( NtHeader, DosHeader, ExportFncOffset ) );

		//Forwarded exports:
		if ( MappedAsImage &&	//Not supported on images that are not mapped
			 //Not supported with ordinals for forwarded export by name
 //Check for forwarded export:
			 ExportFnc > ( (BYTE*)ExportDirectory ) &&
			 ExportFnc < ( (BYTE*)ExportDirectory + ExportDirectorySize ) )
		{
			//for example inside the Kernelbase.dll's export table
			//NTDLL.RtlDecodePointer
			//It could also forward an ordinal
			//NTDLL.#123
			char* ForwardedString = (char*)ExportFnc;
			DWORD ForwardedStringLen = (DWORD)util::fast_strlen( ForwardedString ) + 1;
			if ( ForwardedStringLen >= 256 )
				continue;
			char szForwardedLibraryName[256];
			memcpy( szForwardedLibraryName, ForwardedString, ForwardedStringLen );
			char* ForwardedFunctionName = NULL;
			char* ForwardedFunctionOrdinal = NULL;
			for ( DWORD s = 0; s < ForwardedStringLen; s++ )
			{
				if ( szForwardedLibraryName[s] == '.' )
				{
					szForwardedLibraryName[s] = NULL;
					ForwardedFunctionName = &ForwardedString[s + 1];
					break;
				}
			}

			//forwarded by ordinal
			if ( ForwardedFunctionName != nullptr && ForwardedFunctionName[0] == '#' )
			{
				ForwardedFunctionOrdinal = ForwardedFunctionName + 1;
				ForwardedFunctionName = NULL;
			}
			if ( ForwardedFunctionName )
			{
				if ( strcmp( lpProcName, ForwardedFunctionName ) != NULL )
					continue;

				HMODULE hForwardedDll = LoadLibraryA( szForwardedLibraryName );
				FARPROC ForwardedFunction = (FARPROC)util::find_export_address( hForwardedDll, ForwardedFunctionName, MappedAsImage );
				return (FARPROC)ForwardedFunction;
			}
			else
				if ( ForwardedFunctionOrdinal && ProcOrdinal < 0xFFFF )
				{
					DWORD ForwardedOrdinal = atoi( ForwardedFunctionOrdinal );
					if ( ForwardedOrdinal > 0xFFFF ||
						 ForwardedOrdinal == 0 ||
						 ForwardedOrdinal != ProcOrdinal )
						continue;

					HMODULE hForwardedDll = LoadLibraryA( szForwardedLibraryName );
					FARPROC ForwardedFunction = (FARPROC)util::find_export_address( hForwardedDll, (char*)( ForwardedOrdinal & 0xFFFF ), MappedAsImage );
					return (FARPROC)ForwardedFunction;
				}
				else
					continue;
		}

		if ( (ULONG_PTR)lpProcName > 0xFFFF && strcmp( lpProcName, ProcNamePtr ) == NULL )
			return (FARPROC)ExportFnc;
		else
		{
			if ( ( OrdinalIndex + 1 ) == ProcOrdinal )
				return (FARPROC)ExportFnc;
		}
	}
	return nullptr;
}

std::string util::ConvertWideToANSI( const std::wstring& wstr )
{
	const int count = WideCharToMultiByte( CP_ACP, 0, wstr.c_str( ), wstr.length( ), NULL, 0, NULL, NULL );
	std::string str( count, 0 );
	WideCharToMultiByte( CP_ACP, 0, wstr.c_str( ), -1, &str[0], count, NULL, NULL );
	return str;
}

std::wstring util::ConvertANSIToWide( const std::string& str )
{
	const int count = MultiByteToWideChar( CP_ACP, 0, str.c_str( ), str.length( ), NULL, 0 );
	std::wstring wstr( count, 0 );
	MultiByteToWideChar( CP_ACP, 0, str.c_str( ), str.length( ), &wstr[0], count );
	return wstr;
}

HINSTANCE __stdcall util::get_module_base_wchar( _In_opt_ LPCWSTR lpModuleName )
{
	struct CLIENT_ID
	{
		HANDLE UniqueProcess;
		HANDLE UniqueThread;
	};

	//https://processhacker.sourceforge.io/doc/ntpebteb_8h_source.html#l00166
	struct TEB
	{
		NT_TIB NtTib;
		PVOID EnvironmentPointer;
		CLIENT_ID ClientId;
		PVOID ActiveRpcHandle;
		PVOID ThreadLocalStoragePointer;
		struct PEB* ProcessEnvironmentBlock;
		//...
	};

	//https://processhacker.sourceforge.io/doc/ntpsapi_8h_source.html#l00063
	struct PEB_LDR_DATA
	{
		ULONG Length;
		BOOLEAN Initialized;
		HANDLE SsHandle;
		LIST_ENTRY InLoadOrderModuleList;
		LIST_ENTRY InMemoryOrderModuleList;
		LIST_ENTRY InInitializationOrderModuleList;
		PVOID EntryInProgress;
		BOOLEAN ShutdownInProgress;
		HANDLE ShutdownThreadId;
	};
	//https://processhacker.sourceforge.io/doc/ntpebteb_8h_source.html#l00008
	struct PEB
	{
		BOOLEAN InheritedAddressSpace;
		BOOLEAN ReadImageFileExecOptions;
		BOOLEAN BeingDebugged;
		union
		{
			BOOLEAN BitField;
			struct
			{
				BOOLEAN ImageUsesLargePages : 1;
				BOOLEAN IsProtectedProcess : 1;
				BOOLEAN IsImageDynamicallyRelocated : 1;
				BOOLEAN SkipPatchingUser32Forwarders : 1;
				BOOLEAN IsPackagedProcess : 1;
				BOOLEAN IsAppContainer : 1;
				BOOLEAN IsProtectedProcessLight : 1;
				BOOLEAN SpareBits : 1;
			};
		};
		HANDLE Mutant;
		PVOID ImageBaseAddress;
		PEB_LDR_DATA* Ldr;
		//...
	};
	struct UNICODE_STRING
	{
		USHORT Length;
		USHORT MaximumLength;
		PWCH Buffer;
	};
	//https://processhacker.sourceforge.io/doc/ntldr_8h_source.html#l00102
	struct LDR_DATA_TABLE_ENTRY
	{
		LIST_ENTRY InLoadOrderLinks;
		LIST_ENTRY InMemoryOrderLinks;
		union
		{
			LIST_ENTRY InInitializationOrderLinks;
			LIST_ENTRY InProgressLinks;
		};
		PVOID DllBase;
		PVOID EntryPoint;
		ULONG SizeOfImage;
		UNICODE_STRING FullDllName;
		UNICODE_STRING BaseDllName;
		//...
	};

	PEB* ProcessEnvironmentBlock = ( (PEB*)( (TEB*)( (TEB*)NtCurrentTeb( ) )->ProcessEnvironmentBlock ) );
	if ( lpModuleName == nullptr )
		return (HMODULE)( ProcessEnvironmentBlock->ImageBaseAddress );

	PEB_LDR_DATA* Ldr = ProcessEnvironmentBlock->Ldr;

	LIST_ENTRY* ModuleLists[3] = { 0,0,0 };
	ModuleLists[0] = &Ldr->InLoadOrderModuleList;
	ModuleLists[1] = &Ldr->InMemoryOrderModuleList;
	ModuleLists[2] = &Ldr->InInitializationOrderModuleList;
	for ( int j = 0; j < 3; j++ )
	{
		for ( LIST_ENTRY* pListEntry = ModuleLists[j]->Flink;
			  pListEntry != ModuleLists[j];
			  pListEntry = pListEntry->Flink )
		{
			LDR_DATA_TABLE_ENTRY* pEntry = (LDR_DATA_TABLE_ENTRY*)( (BYTE*)pListEntry - sizeof( LIST_ENTRY ) * j ); //= CONTAINING_RECORD( pListEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks );

			if ( _wcsicmp( pEntry->BaseDllName.Buffer, lpModuleName ) == 0 )
				return (HMODULE)pEntry->DllBase;

			auto GetFileNameFromPath = []( wchar_t* Path )
			{
				wchar_t* LastSlash = NULL;
				for ( DWORD i = 0; Path[i] != NULL; i++ )
				{
					if ( Path[i] == '\\' )
						LastSlash = &Path[i + 1];
				}
				return LastSlash;
			};

			wchar_t* FileName = GetFileNameFromPath( pEntry->FullDllName.Buffer );
			if ( !FileName )
				continue;

			if ( _wcsicmp( FileName, lpModuleName ) == 0 )
				return (HMODULE)pEntry->DllBase;

			auto RemoveFileExtension = []( wchar_t* FullFileName, wchar_t* OutputBuffer, DWORD OutputBufferSize )
			{
				wchar_t* LastDot = NULL;
				for ( DWORD i = 0; FullFileName[i] != NULL; i++ )
					if ( FullFileName[i] == '.' )
						LastDot = &FullFileName[i];

				for ( DWORD j = 0; j < OutputBufferSize; j++ )
				{
					OutputBuffer[j] = FullFileName[j];
					if ( &FullFileName[j] == LastDot )
					{
						OutputBuffer[j] = NULL;
						break;
					}
				}
				OutputBuffer[OutputBufferSize - 1] = NULL;
				return OutputBuffer;
			};

			wchar_t FileNameWithoutExtension[256];
			RemoveFileExtension( FileName, FileNameWithoutExtension, 256 );

			if ( _wcsicmp( FileNameWithoutExtension, lpModuleName ) == 0 )
				return (HMODULE)pEntry->DllBase;
		}
	}
	return nullptr;
}

HINSTANCE __stdcall util::get_module_base_ansi( _In_opt_ LPCSTR lpModuleName )
{
	if ( !lpModuleName ) return NULL;

	const auto wide_str = util::ConvertANSIToWide( lpModuleName );
	const auto ret = util::get_module_base_wchar( wide_str.c_str( ) );
	return ret;
}

void* util::get_interface( const char* dllname, const char* interfacename ) {
	const auto module_handle = util::get_module_base_ansi( dllname );
	if ( module_handle ) {
		typedef void* ( *CreateInterfaceFn )( const char* pName, int* pReturnCode );
		const auto create_interface = reinterpret_cast<CreateInterfaceFn>( util::find_export_address( module_handle, "CreateInterface", true ) );

		return create_interface( interfacename, nullptr );
	}
	return nullptr;
}

void* util::get_interface( HINSTANCE module_handle, const char* interfacename ) {
	if ( module_handle ) {
		typedef void* ( *CreateInterfaceFn )( const char* pName, int* pReturnCode );
		const auto create_interface = reinterpret_cast<CreateInterfaceFn>( util::find_export_address( module_handle, "CreateInterface", true ) );

		return create_interface( interfacename, nullptr );
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

	int index = 0;
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
	const auto start = std::chrono::high_resolution_clock::now( );

	const auto begin = (uintptr_t)util::get_module_base_ansi( smodule );
	const auto dwSizeOfImage = PIMAGE_NT_HEADERS( (uint8_t*)( begin + PIMAGE_DOS_HEADER( begin )->e_lfanew ) )->OptionalHeader.SizeOfImage;
	const auto end = begin + dwSizeOfImage;

	if ( const auto result = reinterpret_cast<std::uintptr_t>( find_pattern_internal( begin, end, format ? ParseCombo( pattern ).c_str( ) : pattern ) ); result ) {
		const auto duration = duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now( ) - start );
		std::cout << name << ": " << std::uppercase << std::hex; util::set_console_color( 0xA );
		std::cout << "0x" << result; util::set_console_color( );
		std::cout << " (" << std::dec << duration.count( ) << "ms)" << std::endl;
		return result;
	}
	std::cout << name << " not found :(" << std::endl;
	return NULL;
}

std::uintptr_t util::find_pattern( HINSTANCE pmodule, const char* pattern, const std::string_view& name, bool format )
{
	const auto start = std::chrono::high_resolution_clock::now( );

	const auto begin = (uintptr_t)pmodule;
	const auto dwSizeOfImage = PIMAGE_NT_HEADERS( (uint8_t*)( begin + PIMAGE_DOS_HEADER( begin )->e_lfanew ) )->OptionalHeader.SizeOfImage;
	const auto end = begin + dwSizeOfImage;

	if ( const auto result = reinterpret_cast<std::uintptr_t>( find_pattern_internal( begin, end, format ? ParseCombo( pattern ).c_str( ) : pattern ) ); result ) {
		const auto duration = duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now( ) - start );
		std::cout << name << ": " << std::uppercase << std::hex; util::set_console_color( 0xA );
		std::cout << "0x" << result; util::set_console_color( );
		std::cout << " (" << std::dec << duration.count( ) << "ms)" << std::endl;
		return result;
	}
	std::cout << name << " not found :(" << std::endl;
	return NULL;
}

uintptr_t util::get_absolute_address( uintptr_t instruction_ptr, int offset, int size )
{
	if ( size == -1 )
		size = instruction_ptr + sizeof( uint32_t );
	const auto result = instruction_ptr + *reinterpret_cast<int32_t*>( instruction_ptr + offset ) + size;
	return IsValidPtr( (void*)result ) ? result : 0;
}