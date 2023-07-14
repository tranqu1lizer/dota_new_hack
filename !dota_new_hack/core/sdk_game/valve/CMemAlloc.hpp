#pragma once

class CMemAlloc : VClass {
public:
	static CMemAlloc* GetInstance( ) {
		static CMemAlloc* p_mem_alloc = nullptr;
		if ( !p_mem_alloc ) p_mem_alloc = *reinterpret_cast<CMemAlloc**>( util::find_export_address( util::get_module_base_wchar( L"tier0.dll" ), "g_pMemAlloc" ) );

		return p_mem_alloc;
	}

	void* allocate( std::size_t size ) {
		return GetVF<void* ( __thiscall* )( CMemAlloc*, std::size_t )>( 1 )( this, size );
	}

	template<typename T>
	T* allocate( ) {
		T obj{};
		auto mem = CallVFunc<1, T*>( sizeof( T ) );
		memcpy( mem, &obj, sizeof( T ) );
		return mem;
	}

	void* AllocateMemoryChunk( std::size_t size ) {
		return GetVF<void* ( __thiscall* )( CMemAlloc*, std::size_t )>( 2 )( this, size );
	}

	void* ReAlloc( const void* p, std::size_t size ) {
		return GetVF<void* ( __thiscall* )( CMemAlloc*, const void*, std::size_t )>( 3 )( this, p, size );
	}

	void Free( const void* p ) {
		return GetVF<void( __thiscall* )( CMemAlloc*, const void* )>( 5 )( this, p );
	}

	std::size_t GetSize( const void* p ) {
		return GetVF<std::size_t( __thiscall* )( CMemAlloc*, const void* )>( 21 )( this, p );
	}
};
