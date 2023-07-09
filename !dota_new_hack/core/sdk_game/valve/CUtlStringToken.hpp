#pragma once

#include <cstdint>

#define DEBUG_STRINGTOKENS 0

class CUtlStringToken
{
public:
	uint32_t m_nHashCode;
#if DEBUG_STRINGTOKENS
	char const* m_pDebugName;
#endif

	bool operator==( CUtlStringToken const& other ) const
	{
		return ( other.m_nHashCode == m_nHashCode );
	}

	bool operator!=( CUtlStringToken const& other ) const
	{
		return ( other.m_nHashCode != m_nHashCode );
	}

	bool operator<( CUtlStringToken const& other ) const
	{
		return ( m_nHashCode < other.m_nHashCode );
	}


	/// access to the hash code for people who need to store thse as 32-bits, regardless of the
	/// setting of DEBUG_STRINGTOKENS (for instance, for atomic operations).
	uint32_t GetHashCode( void ) const { return m_nHashCode; }

	void SetHashCode( uint32_t nCode ) { m_nHashCode = nCode; }

	CUtlStringToken( ) { m_nHashCode = 0; }
};