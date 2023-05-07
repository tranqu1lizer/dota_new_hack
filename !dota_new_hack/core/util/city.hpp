#pragma once

#include <string>
#include <string_view>
#include <string.h>
#include <cstdint>
#include <utility>

typedef uint8_t uint8;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef std::pair<uint64, uint64> uint128;

inline uint64 Uint128Low64( const uint128& x ) { return x.first; }
inline uint64 Uint128High64( const uint128& x ) { return x.second; }

// Hash function for a byte array.  Most useful in 32-bit binaries.
uint32 CityHash32( const char* buf, size_t len );

// Some custom wrappers
__forceinline uint32 CityHash32( const std::string& str ) {
	return CityHash32( str.c_str( ), str.size( ) );
};
__forceinline uint32 CityHash32( const std::string_view& str ) {
	return CityHash32( str.data( ), str.size( ) );
};

// Hash 128 input bits down to 64 bits of output.
// This is intended to be a reasonably good hash function.
inline uint64 Hash128to64( const uint128& x ) {
	// Murmur-inspired hashing.
	const uint64 kMul = 0x9ddfea08eb382d69ULL;
	uint64 a = ( Uint128Low64( x ) ^ Uint128High64( x ) ) * kMul;
	a ^= ( a >> 47 );
	uint64 b = ( Uint128High64( x ) ^ a ) * kMul;
	b ^= ( b >> 47 );
	b *= kMul;
	return b;
}