#include <iostream>
#include <vector>
#include <windows.h>

namespace util {
	void* get_interface( const char* dllname, const char* interfacename ) {
		const auto module_handle = GetModuleHandleA( dllname );
		if ( module_handle ) {
			const auto export_address = GetProcAddress( module_handle, "CreateInterface" );
			return reinterpret_cast<void* ( * )( const char* pName, int* pReturnCode )>( export_address )( interfacename, nullptr );
		}
		return nullptr;
	}

	void allocate_console( ) {
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
}

template <class T>
class CUtlVector {
public:
	CUtlVector( ) {
		m_Size = 0;
		m_pElements = nullptr;
	}

	T& Element( int i );
	const T& Element( int i ) const;

	T& operator[]( int i ) {
		return m_pElements[i];
	}

	T* begin( ) const
	{
		return m_pElements;
	}

	T* end( ) const noexcept
	{
		return m_pElements + m_Size;
	}

	[[nodiscard]] int Count( ) const {
		return m_Size;
	}

	[[nodiscard]] std::vector<T> stl_vector( ) {
		auto result = std::vector<T>{};
		result.reserve( m_Size );
		for ( int i = 0; i < m_Size; i++ )
			result.push_back( m_pElements[i] );
		return result;
	}

	T& last( ) {
		return m_pElements[m_Size - 1];
	}

	int	m_Size; // 0x0 
	T* m_pElements; // 0x8 
	int	m_nAllocationCount; // 0x10 
};

using UtlTsHashHandleT = std::uint64_t;

class CUtlMemoryPool {
public:
	// returns number of allocated blocks
	int BlockSize( ) const {
		return m_blocks_per_blob_;
	}
	int Count( ) const {
		return m_block_allocated_size_;
	}
	int PeakCount( ) const {
		return m_peak_alloc_;
	}
private:
	[[maybe_unused]] std::int32_t m_block_size_ = 0; // 0x0558
	std::int32_t m_blocks_per_blob_ = 0; // 0x055C
	[[maybe_unused]] std::int32_t m_grow_mode_ = 0; // 0x0560
	[[maybe_unused]] std::int32_t m_blocks_allocated_ = 0; // 0x0564
	std::int32_t m_block_allocated_size_ = 0; // 0x0568
	std::int32_t m_peak_alloc_ = 0; // 0x056C
};

template <class T, class Keytype = std::uint64_t>
class CUtlTSHash {
public:
	// Invalid handle.
	static UtlTsHashHandleT InvalidHandle( void ) {
		return static_cast<UtlTsHashHandleT>( 0 );
	}

	// Returns the number of elements in the hash table
	[[nodiscard]] int BlockSize( ) const {
		return m_entry_memory_.BlockSize( );
	}
	[[nodiscard]] int Count( ) const {
		return m_entry_memory_.Count( );
	}

	// Returns elements in the table
	std::vector<T> GetElements( void );
public:
	// Templatized for memory tracking purposes
	template <typename DataT>
	struct HashFixedDataInternalT {
		Keytype m_ui_key;
		HashFixedDataInternalT<DataT>* m_next;
		DataT m_data;
	};

	using HashFixedDataT = HashFixedDataInternalT<T>;

	// Templatized for memory tracking purposes
	template <typename DataT>
	struct HashFixedStructDataInternalT {
		DataT m_data;
		Keytype m_ui_key;
		char pad_0x0020[0x8];
	};

	using HashFixedStructDataT = HashFixedStructDataInternalT<T>;

	struct HashStructDataT {
		char pad_0x0000[0x10]; // 0x0000
		std::array<HashFixedStructDataT, 256> m_list;
	};

	struct HashAllocatedDataT {
	public:
		auto GetList( ) {
			return m_list_;
		}
	private:
		char pad_0x0000[0x18]; // 0x0000
		std::array<HashFixedDataT, 128> m_list_;
	};

	// Templatized for memory tracking purposes
	template <typename DataT>
	struct HashBucketDataInternalT {
		DataT m_data;
		HashFixedDataInternalT<DataT>* m_next;
		Keytype m_ui_key;
	};

	using HashBucketDataT = HashBucketDataInternalT<T>;

	struct HashUnallocatedDataT {
		HashUnallocatedDataT* m_next_ = nullptr; // 0x0000
		Keytype m_6114; // 0x0008
		Keytype m_ui_key; // 0x0010
		Keytype m_i_unk_1; // 0x0018
		std::array<HashBucketDataT, 256> m_current_block_list; // 0x0020
	};

	struct HashBucketT {
		HashStructDataT* m_struct_data = nullptr;
		void* m_mutex_list = nullptr;
		HashAllocatedDataT* m_allocated_data = nullptr;
		HashUnallocatedDataT* m_unallocated_data = nullptr;
	};

	CUtlMemoryPool m_entry_memory_;
	HashBucketT m_buckets_;
	bool m_needs_commit_ = false;
};

template <class T, class Keytype>
std::vector<T> CUtlTSHash<T, Keytype>::GetElements( void ) {
	std::vector<T> list;

	const int n_count = Count( );
	auto n_index = 0;
	auto& unallocated_data = m_buckets_.m_unallocated_data;
	for ( auto element = unallocated_data; element; element = element->m_next_ ) {
		for ( auto i = 0; i < BlockSize( ) && i != n_count; i++ ) {
			list.emplace_back( element->m_current_block_list.at( i ).m_data );
			n_index++;

			if ( n_index >= n_count )
				break;
		}
	}
	return list;
}

class NormalClass
{
public:
	template<typename T>
	T& Field( int offset ) {
		return *(T*)( (uintptr_t)this + offset );
	}

	template<class T>
	auto& Member( std::ptrdiff_t offset ) noexcept
	{
		return *reinterpret_cast<T*>( reinterpret_cast<std::uintptr_t>( this ) + offset );
	}

	template<class T>
	const auto& Member( std::ptrdiff_t offset ) const noexcept
	{
		return *reinterpret_cast<const T*>( reinterpret_cast<std::uintptr_t>( this ) + offset );
	}

	template<typename T>
	T* MemberNotPtr( std::ptrdiff_t offset ) {
		return reinterpret_cast<T*>( reinterpret_cast<std::uintptr_t>( this ) + offset );
	}

	template<typename T>
	auto MemberInline( std::ptrdiff_t offset ) {
		return (T*)( (uintptr_t)this + offset );
	}
};

class VFTable {
	void* VTablePtr{ nullptr };
public:
	VFTable( ) = default;
	explicit VFTable( void* vmt ) noexcept
		:
		VTablePtr{ vmt }
	{}

	VFTable( const VFTable& ) = default;
	VFTable& operator=( const VFTable& ) = default;

	VFTable( VFTable&& other ) noexcept
		:
		VTablePtr{ other.VTablePtr }
	{
		other.VTablePtr = nullptr;
	}
	VFTable& operator=( VFTable&& other ) noexcept
	{
		VTablePtr = other.VTablePtr;
		other.VTablePtr = nullptr;
		return *this;
	}
	~VFTable( ) = default;

public:
	bool is_valid( ) const noexcept
	{
		return VTablePtr != nullptr;
	}

	void*& GetFunctionAt( std::size_t index ) noexcept
	{
		return *( static_cast<void**>( VTablePtr ) + index );
	}

	const void*& GetFunctionAt( std::size_t index ) const noexcept
	{
		return *( static_cast<const void**>( VTablePtr ) + index );
	}

	operator void* ( ) noexcept
	{
		return VTablePtr;
	}

	template<typename ReturnType = decltype( VTablePtr )>
	ReturnType GetPtr( ) const noexcept
	{
		return SmartCast<ReturnType>( VTablePtr );
	}
};

class VClass : public NormalClass
{
	VFTable VirtualMethodsTable{};
public:
	VClass( ) = default;

	explicit VClass( const VFTable& vftable ) noexcept : VirtualMethodsTable{ vftable } {}

	template<std::size_t FunctionIndex, typename ReturnType = std::uintptr_t, typename ... ArgTypes>
	ReturnType CallVFunc( ArgTypes... Args ) const
	{
		const auto func = VirtualMethodsTable.GetFunctionAt( FunctionIndex );
		return ( static_cast<ReturnType( __thiscall* )( const VClass*, ArgTypes... )>( func ) )( this, Args... );
	}

	template <typename T = std::uintptr_t>
	inline T GetVF( int Index )
	{
		return ( *reinterpret_cast<T**>( this ) )[Index];
	}

	const VFTable& GetVMT( ) const noexcept
	{
		return VirtualMethodsTable;
	}
};

#define CSCHEMATYPE_GETSIZES_INDEX 3
#define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0x190
#define SCHEMASYSTEMTYPESCOPE_OFF1 0x450
#define SCHEMASYSTEMTYPESCOPE_OFF2 0x2804

class CSchemaClassInfo;
class CSchemaSystemTypeScope;
class CSchemaType;

struct SchemaMetadataEntryData_t;
struct SchemaMetadataSetData_t;
struct SchemaClassInfoData_t;

using SchemaString_t = const char*;

template <typename T>
struct SchemaArray {
public:
	T* begin( ) const {
		return m_data;
	}

	T* end( ) const {
		return m_data + m_size;
	}

	T* m_data;
	unsigned int m_size;
};

struct CSchemaNetworkValue {
	union {
		const char* m_sz_value;
		int m_n_value;
		float m_f_value;
		std::uintptr_t m_p_value;
	};
};

struct CSchemaClassBinding {
	CSchemaClassBinding* parent;
	const char* m_binary_name; // ex: C_World
	const char* m_module_name; // ex: libclient.so
	const char* m_class_name; // ex: client
	void* m_class_info_old_synthesized;
	CSchemaClassInfo* m_class_info;
	void* m_this_module_binding_pointer;
	CSchemaType* m_schema_type;
};

struct SchemaEnumeratorInfoData_t {
	SchemaString_t m_name;

	union {
		unsigned char m_value_char;
		unsigned short m_value_short;
		unsigned int m_value_int;
		unsigned long long m_value;
	};

	char pad_0x0010[0x10]; // 0x0010
};

class CSchemaEnumInfo {
public:
	SchemaEnumeratorInfoData_t m_field_;
};

class CSchemaEnumBinding {
public:
	virtual const char* GetBindingName( ) = 0;
	virtual CSchemaClassBinding* AsClassBinding( ) = 0;
	virtual CSchemaEnumBinding* AsEnumBinding( ) = 0;
	virtual const char* GetBinaryName( ) = 0;
	virtual const char* GetProjectName( ) = 0;
public:
	char* m_binding_name_; // 0x0008
	char* m_dll_name_; // 0x0010
	std::int8_t m_align_; // 0x0018
	char pad_0x0019[0x3]; // 0x0019
	std::int16_t m_size_; // 0x001C
	std::int16_t m_flags_; // 0x001E
	SchemaEnumeratorInfoData_t* m_enum_info_;
	char pad_0x0028[0x8]; // 0x0028
	CSchemaSystemTypeScope* m_type_scope_; // 0x0030
	char pad_0x0038[0x8]; // 0x0038
	std::int32_t m_i_unk1_; // 0x0040
};

enum SchemaClassFlags_t {
	SCHEMA_CLASS_HAS_VIRTUAL_MEMBERS = 1,
	SCHEMA_CLASS_IS_ABSTRACT = 2,
	SCHEMA_CLASS_HAS_TRIVIAL_CONSTRUCTOR = 4,
	SCHEMA_CLASS_HAS_TRIVIAL_DESTRUCTOR = 8,
	SCHEMA_CLASS_TEMP_HACK_HAS_NOSCHEMA_MEMBERS = 16,
	SCHEMA_CLASS_TEMP_HACK_HAS_CONSTRUCTOR_LIKE_METHODS = 32,
	SCHEMA_CLASS_TEMP_HACK_HAS_DESTRUCTOR_LIKE_METHODS = 64,
	SCHEMA_CLASS_IS_NOSCHEMA_CLASS = 128,
};

enum ETypeCategory {
	Schema_Builtin = 0,
	Schema_Ptr = 1,
	Schema_Bitfield = 2,
	Schema_FixedArray = 3,
	Schema_Atomic = 4,
	Schema_DeclaredClass = 5,
	Schema_DeclaredEnum = 6,
	Schema_None = 7
};

enum EAtomicCategory {
	Atomic_Basic,
	Atomic_T,
	Atomic_CollectionOfT,
	Atomic_TT,
	Atomic_I,
	Atomic_None
};

class CSchemaType {
public:
	bool GetSizes( int* out_size1, uint8_t* unk_probably_not_size ) {
		return reinterpret_cast<int( __thiscall* )( void*, int*, uint8_t* )>( _vtable[CSCHEMATYPE_GETSIZES_INDEX] )( this, out_size1, unk_probably_not_size );
	}
public:
	bool GetSize( int* out_size ) {
		uint8_t smh = 0;
		return GetSizes( out_size, &smh );
	}
public:
	uintptr_t* _vtable; // 0x0000
	const char* m_name_; // 0x0008

	CSchemaSystemTypeScope* m_type_scope_; // 0x0010
	uint8_t type_category; // ETypeCategory 0x0018
	uint8_t atomic_category; // EAtomicCategory 0x0019

	// find out to what class pointer points.
	CSchemaType* GetRefClass( ) {
		if ( type_category != Schema_Ptr )
			return nullptr;

		auto ptr = m_schema_type_;
		while ( ptr && ptr->type_category == ETypeCategory::Schema_Ptr )
			ptr = ptr->m_schema_type_;

		return ptr;
	}

	struct array_t {
		uint32_t array_size;
		uint32_t unknown;
		CSchemaType* element_type_;
	};

	struct atomic_t { // same goes for CollectionOfT
		uint64_t gap[2];
		CSchemaType* template_typename;
	};

	struct atomic_tt {
		uint64_t gap[2];
		CSchemaType* templates[2];
	};

	struct atomic_i {
		uint64_t gap[2];
		uint64_t integer;
	};

	// this union depends on CSchema implementation, all members above
	// is from base class ( CSchemaType )
	union // 0x020
	{
		CSchemaType* m_schema_type_;
		CSchemaClassInfo* m_class_info;
		CSchemaEnumBinding* m_enum_binding_;
		array_t m_array_;
		atomic_t m_atomic_t_;
		atomic_tt m_atomic_tt_;
		atomic_i m_atomic_i_;
	};
};
static_assert( offsetof( CSchemaType, m_schema_type_ ) == 0x20 );

struct SchemaMetadataEntryData_t {
	SchemaString_t m_name;
	CSchemaNetworkValue* m_value;
	// CSchemaType* m_pDataType;
	// void* unaccounted;
};

struct SchemaMetadataSetData_t {
	SchemaMetadataEntryData_t m_static_entries;
};

struct SchemaClassFieldData_t {
	SchemaString_t m_name; // 0x0000
	CSchemaType* m_type; // 0x0008
	std::int32_t m_single_inheritance_offset; // 0x0010
	std::int32_t m_metadata_size; // 0x0014
	SchemaMetadataEntryData_t* m_metadata; // 0x0018
};

struct SchemaStaticFieldData_t {
	const char* name; // 0x0000
	CSchemaType* m_type; // 0x0008
	void* m_instance; // 0x0010
	char pad_0x0018[0x10]; // 0x0018
};

struct SchemaBaseClassInfoData_t {
	unsigned int m_offset;
	CSchemaClassInfo* m_class;
};

// Classes
struct SchemaClassInfoData_t {
	char pad_0x0000[0x8]; // 0x0000

	const char* m_name; // 0x0008
	char* m_module; // 0x0010

	int m_size; // 0x0018
	std::int16_t m_align; // 0x001C

	std::int16_t m_static_size; // 0x001E
	std::int16_t m_metadata_size; // 0x0020
	std::int16_t m_i_unk1; // 0x0022
	std::int16_t m_i_unk2; // 0x0024
	std::int16_t m_i_unk3; // 0x0026

	SchemaClassFieldData_t* m_fields; // 0x0028

	SchemaStaticFieldData_t* m_static_fields; // 0x0030
	SchemaBaseClassInfoData_t* m_schema_parent; // 0x0038

	char pad_0x0038[0x10]; // 0x0038

	SchemaMetadataSetData_t* m_metadata; // 0x0048
	CSchemaSystemTypeScope* m_type_scope; // 0x0050
	CSchemaType* m_shema_type; // 0x0058
	SchemaClassFlags_t m_class_flags : 8; // 0x0060
};

class CSchemaClassInfo : public SchemaClassInfoData_t {
public:
	bool GetMetaStrings( const char* metaName, std::vector<const char**>& strings );

	unsigned int CalculateInheritanceDataSize( bool ignoreVirtuals = false );

	bool DependsOn( CSchemaClassInfo* other );
	bool InheritsFrom( CSchemaClassInfo* other );
	bool UsesClass( CSchemaClassInfo* other );
	bool InheritsVirtuals( );

	void FillClassFieldsList( std::vector<SchemaClassFieldData_t*>& fields );
	void FillInheritanceList( std::vector<const char*>& inheritance );
private:
};

#include <array>

class CSchemaSystemTypeScope : VClass {
	std::array<char, 256> name{};

public:
	CSchemaClassInfo* FindDeclaredClass( const char* class_name ) {
		return CallVFunc<2, CSchemaClassInfo*>( class_name );
	}

	CSchemaEnumBinding* FindDeclaredEnum( const char* name ) {
		return CallVFunc<3, CSchemaEnumBinding*>( name );
	}

	CSchemaType* FindSchemaTypeByName( const char* name, std::uintptr_t* schema ) {
		return CallVFunc<4, CSchemaType*>( name, schema );
	}

	CSchemaType* FindTypeDeclaredClass( const char* name ) {
		return CallVFunc<5, CSchemaType*>( name );
	}

	CSchemaType* FindTypeDeclaredEnum( const char* name ) {
		return CallVFunc<6, CSchemaType*>( name );
	}

	CSchemaClassBinding* FindRawClassBinding( const char* name ) {
		return CallVFunc<7, CSchemaClassBinding*>( name );
	}

	CSchemaEnumBinding* FindRawEnumBinding( const char* name ) {
		return CallVFunc<8, CSchemaEnumBinding*>( name );
	}

	std::string_view GetName( ) const noexcept
	{
		return { name.data( ) };
	}

	CUtlTSHash<CSchemaClassBinding*> GetClasses( ) const noexcept
	{
		return Member<CUtlTSHash<CSchemaClassBinding*>>( 0x558 );
	}

	CUtlTSHash<CSchemaEnumBinding*> GetEnums( ) const noexcept
	{
		return Member<CUtlTSHash<CSchemaEnumBinding*>>( 0x2DA0 );
	}
};

class CSchemaSystem : VClass {
	static auto GetInstanceImpl( )
	{
		static CSchemaSystem* inst = nullptr;
		if ( !inst ) inst = static_cast<CSchemaSystem*>( util::get_interface( "schemasystem.dll", "SchemaSystem_001" ) );

		return inst;
	}
public:
	static auto& GetInstance( )
	{
		return *GetInstanceImpl( );
	}

	inline CSchemaSystemTypeScope* GlobalTypeScope( void ) {
		return CallVFunc<11, CSchemaSystemTypeScope*>( );
	}

	inline CSchemaSystemTypeScope* FindTypeScopeForModule( const char* m_module_name ) {
		return CallVFunc<13, CSchemaSystemTypeScope*>( m_module_name );
	}

	[[nodiscard]] CUtlVector<CSchemaSystemTypeScope*> GetTypeScopes( void ) const {
		return Member<CUtlVector<CSchemaSystemTypeScope*>>( SCHEMASYSTEM_TYPE_SCOPES_OFFSET );
	}
};

template<char... characters>
constexpr std::size_t helper_string_find_either( const std::string_view& str )
{
	for ( std::size_t pos{}; const auto & character : str )
	{
		if ( ( ( character == characters ) || ... ) )
			return pos;
		++pos;
	}
	return std::string_view::npos;
}

constexpr auto split_path_into_relative_paths( const std::string_view& FullPath )
{
	std::vector<std::string> result{};
	std::string_view FullPath_copy{ FullPath };
	std::size_t pos{ helper_string_find_either<'\\', '/'>( FullPath_copy ) };
	while ( pos != std::string_view::npos )
	{
		if ( const auto path = FullPath_copy.substr( 0, pos ); !path.empty( ) )
			result.push_back( std::string{ path } );
		FullPath_copy = FullPath_copy.substr( pos + 1 );
		pos = helper_string_find_either<'\\', '/'>( FullPath_copy );
	}
	result.push_back( std::string{ FullPath_copy } );
	return result;
}

class schema {
public:
	static void dump_class_offsets( const std::string& name ) {
		if ( const auto& paths = split_path_into_relative_paths( name ); paths.size( ) == 2 ) {
			const auto& mod = paths.at( 0 );
			const auto& cls = paths.at( 1 );

			if ( const auto module_scope = CSchemaSystem::GetInstance( ).FindTypeScopeForModule( mod.c_str( ) ); module_scope ) {
				if ( const auto class_info = module_scope->FindDeclaredClass( cls.c_str( ) ); class_info ) {
					std::cout << "		class " << class_info->m_name << " (size: 0x" << std::hex << class_info->m_size << ")" << std::endl;
					for ( auto k = 0; k < class_info->m_align; k++ ) {
						if ( const auto& field = &class_info->m_fields[k]; field ) {
							std::cout << field->m_type->m_name_ << " " << field->m_name << ": 0x" << std::hex << field->m_single_inheritance_offset << std::endl;
						}
					}
				}
			}
		}
	}
};