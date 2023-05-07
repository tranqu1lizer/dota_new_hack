#pragma once

//-----------------------------------------------------------------------------
// The CUtlMemory class:
// A growable memory class which doubles in size by default.
//-----------------------------------------------------------------------------
template< class T, class I = int >
class CUtlMemory
{
public:
	CUtlMemory( int nGrowSize = 0, int nInitSize = 0 )
		: m_pMemory( nullptr ), m_nAllocationCount( nInitSize ), m_nGrowSize( nGrowSize )
	{
	}

	class Iterator_t
	{
	public:
		Iterator_t( I i ) : index( i ) {}
		I index;

		bool operator==( const Iterator_t it ) const { return index == it.index; }
		bool operator!=( const Iterator_t it ) const { return index != it.index; }
	};
	Iterator_t First( ) const { return Iterator_t( IsIdxValid( 0 ) ? 0 : InvalidIndex( ) ); }
	Iterator_t Next( const Iterator_t& it ) const { return Iterator_t( IsIdxValid( it.index + 1 ) ? it.index + 1 : InvalidIndex( ) ); }
	I GetIndex( const Iterator_t& it ) const { return it.index; }
	bool IsIdxAfter( I i, const Iterator_t& it ) const { return i > it.index; }
	bool IsValidIterator( const Iterator_t& it ) const { return IsIdxValid( it.index ); }
	Iterator_t InvalidIterator( ) const { return Iterator_t( InvalidIndex( ) ); }

	// element access
	T& operator[]( I i ) { return m_pMemory[(unsigned int)i]; }
	const T& operator[]( I i ) const { return m_pMemory[(unsigned int)i]; }
	T& Element( I i ) { return m_pMemory[(unsigned int)i]; }
	const T& Element( I i ) const { return m_pMemory[(unsigned int)i]; }

	// Can we use this index?
	bool IsIdxValid( I i ) const { return (unsigned int)i < (unsigned int)m_nAllocationCount; }

	// Specify the invalid ('null') index that we'll only return on failure
	static const I INVALID_INDEX = (I)-1; // For use with COMPILE_TIME_ASSERT
	static I InvalidIndex( ) { return INVALID_INDEX; }

	// Gets the base address (can change when adding elements!)
	T* Base( ) { return m_pMemory; }
	const T* Base( ) const { return m_pMemory; }

	// Size
	int NumAllocated( ) const { return m_nAllocationCount; }
	int Count( ) const { return m_nAllocationCount; }

	// is the memory externally allocated?
	bool IsExternallyAllocated( ) const { return ( m_nGrowSize < 0 ); }

	// is the memory read only?
	bool IsReadOnly( ) const { return ( m_nGrowSize == EXTERNAL_CONST_BUFFER_MARKER ); }

protected:
	enum
	{
		EXTERNAL_BUFFER_MARKER = -1,
		EXTERNAL_CONST_BUFFER_MARKER = -2,
	};

	T* m_pMemory;
	int m_nAllocationCount;
	int m_nGrowSize;
};