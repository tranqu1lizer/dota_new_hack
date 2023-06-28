#pragma once

#include <vector>
#include "../NormalClass.hpp"
#include "../../util/util.hpp"
#include <cstddef>
#include <assert.h>

using CUtlSymbolLarge = const char*;

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

	void clear( ) {
		for ( int i = 0; i < this->Count( ); i++ ) {
			FastRemove( i );
		}
	}

	int	m_Size; // 0x0 
	T* m_pElements; // 0x8 
	int	m_nAllocationCount; // 0x10 

	// Adds an element, uses default constructor
	int add_to_head( );
	int add_to_tail( );
	int InsertBefore( int elem );
	int InsertAfter( int elem );
	// Adds an element, uses copy constructor
	int add_to_head( const T& src );
	int add_to_tail( const T& src );
	int InsertBefore( int elem, const T& src );
	int InsertAfter( int elem, const T& src );

	// Grows the vector
	void GrowVector( int num = 1 );

	// Shifts elements....
	void ShiftElementsRight( int elem, int num = 1 );
	void ShiftElementsLeft( int elem, int num = 1 );

	// Element removal
	void FastRemove( int elem ); // doesn't preserve order
	bool FindAndFastRemove( const T& src ); // removes first occurrence of src, doesn't preserve order

	// Finds an element (element needs operator== defined)
	int GetOffset( const T& src ) const;
};

template <typename T>
inline T& CUtlVector<T>::Element( int i ) {
	assert( i < m_Size );
	return m_pElements[i];
}

template <typename T>
inline const T& CUtlVector<T>::Element( int i ) const {
	assert( i < m_Size );
	return m_pElements[i];
}

template <typename T>
void CUtlVector<T>::GrowVector( int num ) {
	m_Size += num;
	if ( m_pElements )
		m_pElements = (T*)CMemAlloc::GetInstance( )->ReAlloc( m_pElements, m_Size * sizeof( T ) );
	else
		m_pElements = (T*)CMemAlloc::GetInstance( )->allocate( m_Size * sizeof( T ) );
}

//-----------------------------------------------------------------------------
// Adds an element, uses default constructor
//-----------------------------------------------------------------------------
template <typename T>
inline int CUtlVector<T>::add_to_head( ) {
	return InsertBefore( 0 );
}

template <typename T>
inline int CUtlVector<T>::add_to_tail( ) {
	return InsertBefore( m_Size );
}

template <typename T>
inline int CUtlVector<T>::InsertAfter( int elem ) {
	return InsertBefore( elem + 1 );
}

template <typename T>
int CUtlVector<T>::InsertBefore( int elem ) {
	// Can insert at the end
	GrowVector( );
	ShiftElementsRight( elem );
	Construct( &Element( elem ) );
	return elem;
}

//-----------------------------------------------------------------------------
// Adds an element, uses copy constructor
//-----------------------------------------------------------------------------
template <typename T>
inline int CUtlVector<T>::add_to_head( const T& src ) {
	return InsertBefore( 0, src );
}

template <typename T>
inline int CUtlVector<T>::add_to_tail( const T& src ) {
	return InsertBefore( m_Size, src );
}

template <typename T>
inline int CUtlVector<T>::InsertAfter( int elem, const T& src ) {
	return InsertBefore( elem + 1, src );
}

template <class T>
inline T* CopyConstruct( T* pMemory, const T& src ) {
	return ::new ( pMemory ) T( src );
}

template <class T>
inline T* Construct( T* pMemory ) {
	return ::new ( pMemory ) T;
}

template <class T, typename ARG1>
inline T* Construct( T* pMemory, ARG1 a1 ) {
	return ::new ( pMemory ) T( a1 );
}

template <class T, typename ARG1, typename ARG2>
inline T* Construct( T* pMemory, ARG1 a1, ARG2 a2 ) {
	return ::new ( pMemory ) T( a1, a2 );
}

template <class T, typename ARG1, typename ARG2, typename ARG3>
inline T* Construct( T* pMemory, ARG1 a1, ARG2 a2, ARG3 a3 ) {
	return ::new ( pMemory ) T( a1, a2, a3 );
}

template <class T, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
inline T* Construct( T* pMemory, ARG1 a1, ARG2 a2, ARG3 a3, ARG4 a4 ) {
	return ::new ( pMemory ) T( a1, a2, a3, a4 );
}

template <class T, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5>
inline T* Construct( T* pMemory, ARG1 a1, ARG2 a2, ARG3 a3, ARG4 a4, ARG5 a5 ) {
	return ::new ( pMemory ) T( a1, a2, a3, a4, a5 );
}

template <typename T>
int CUtlVector<T>::InsertBefore( int elem, const T& src ) {
	GrowVector( );
	ShiftElementsRight( elem );
	CopyConstruct( &Element( elem ), src );
	return elem;
}

//-----------------------------------------------------------------------------
// Shifts elements
//-----------------------------------------------------------------------------
template <typename T>
void CUtlVector<T>::ShiftElementsRight( int elem, int num ) {
	int numToMove = m_Size - elem - num;
	if ( ( numToMove > 0 ) && ( num > 0 ) )
		memmove( &Element( elem + num ), &Element( elem ), numToMove * sizeof( T ) );
}

template <typename T>
void CUtlVector<T>::ShiftElementsLeft( int elem, int num ) {
	int numToMove = m_Size - elem - num;
	if ( ( numToMove > 0 ) && ( num > 0 ) ) {
		memmove( &Element( elem ), &Element( elem + num ), numToMove * sizeof( T ) );

#ifdef _DEBUG
		memset( &Element( m_Size - num ), 0xDD, num * sizeof( T ) );
#endif
	}
}

//-----------------------------------------------------------------------------
// Element removal
//-----------------------------------------------------------------------------
template <typename T>
void CUtlVector<T>::FastRemove( int elem ) {
	Element( elem )->~T( );
	if ( m_Size > 0 ) {
		if ( elem != m_Size - 1 )
			memcpy( &Element( elem ), &Element( m_Size - 1 ), sizeof( T ) );
		--m_Size;
	}
}

template <typename T>
bool CUtlVector<T>::FindAndFastRemove( const T& src ) {
	int elem = GetOffset( src );
	if ( elem != -1 ) {
		FastRemove( elem );
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Finds an element (element needs operator== defined)
//-----------------------------------------------------------------------------
template <typename T>
int CUtlVector<T>::GetOffset( const T& src ) const {
	for ( auto i = 0; i < Count( ); ++i ) {
		if ( Element( i ) == src )
			return i;
	}
	return -1;
}