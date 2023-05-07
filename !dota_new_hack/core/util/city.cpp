#define bswap_32(x) _byteswap_ulong(x)
#define bswap_64(x) _byteswap_uint64(x)

#define uint32_in_expected_order(x) (x)
#define uint64_in_expected_order(x) (x)
#define LIKELY(x) (x)

#include "city.hpp"

#include <algorithm>

static uint32 UNALIGNED_LOAD32( const char* p ) {
	uint32 result;
	memcpy( &result, p, sizeof( result ) );
	return result;
}

static uint32 Fetch32( const char* p ) {
	return uint32_in_expected_order( UNALIGNED_LOAD32( p ) );
}

// Magic numbers for 32-bit hashing.  Copied from Murmur3.
static constexpr uint32 c1 = 0xcc9e2d51;
static constexpr uint32 c2 = 0x1b873593;

// A 32-bit to 32-bit integer hash copied from Murmur3.
static uint32 fmix( uint32 h )
{
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}

static uint32 Rotate32( uint32 val, int shift ) {
	// Avoid shifting by 32: doing so yields an undefined result.
	return shift == 0 ? val : ( ( val >> shift ) | ( val << ( 32 - shift ) ) );
}

#undef PERMUTE3
#define PERMUTE3(a, b, c) do { std::swap(a, b); std::swap(a, c); } while (0)

static uint32 Mur( uint32 a, uint32 h ) {
	// Helper from Murmur3 for combining two 32-bit values.
	a *= c1;
	a = Rotate32( a, 17 );
	a *= c2;
	h ^= a;
	h = Rotate32( h, 19 );
	return h * 5 + 0xe6546b64;
}

static uint32 Hash32Len13to24( const char* s, size_t len ) {
	const auto a = Fetch32( s - 4 + ( len >> 1 ) );
	const auto b = Fetch32( s + 4 );
	const auto c = Fetch32( s + len - 8 );
	const auto d = Fetch32( s + ( len >> 1 ) );
	const auto e = Fetch32( s );
	const auto f = Fetch32( s + len - 4 );
	const auto h = len;

	return fmix( Mur( f, Mur( e, Mur( d, Mur( c, Mur( b, Mur( a, h ) ) ) ) ) ) );
}

static uint32 Hash32Len0to4( const char* s, size_t len ) {
	uint32 b = 0;
	uint32 c = 9;
	for ( size_t i = 0; i < len; i++ ) {
		const signed char v = s[i];
		b = b * c1 + v;
		c ^= b;
	}
	return fmix( Mur( b, Mur( len, c ) ) );
}

static uint32 Hash32Len5to12( const char* s, size_t len ) {
	uint32 a = len, b = len * 5, c = 9, d = b;
	a += Fetch32( s );
	b += Fetch32( s + len - 4 );
	c += Fetch32( s + ( ( len >> 1 ) & 4 ) );
	return fmix( Mur( c, Mur( b, Mur( a, d ) ) ) );
}

uint32 CityHash32( const char* s, size_t len ) {
	if ( len <= 24 ) {
		return len <= 12 ?
			( len <= 4 ? Hash32Len0to4( s, len ) : Hash32Len5to12( s, len ) ) :
			Hash32Len13to24( s, len );
	}

	// len > 24
	uint32 h = len, g = c1 * len, f = g;
	const auto a0 = Rotate32( Fetch32( s + len - 4 ) * c1, 17 ) * c2;
	const auto a1 = Rotate32( Fetch32( s + len - 8 ) * c1, 17 ) * c2;
	const auto a2 = Rotate32( Fetch32( s + len - 16 ) * c1, 17 ) * c2;
	const auto a3 = Rotate32( Fetch32( s + len - 12 ) * c1, 17 ) * c2;
	const auto a4 = Rotate32( Fetch32( s + len - 20 ) * c1, 17 ) * c2;
	h ^= a0;
	h = Rotate32( h, 19 );
	h = h * 5 + 0xe6546b64;
	h ^= a2;
	h = Rotate32( h, 19 );
	h = h * 5 + 0xe6546b64;
	g ^= a1;
	g = Rotate32( g, 19 );
	g = g * 5 + 0xe6546b64;
	g ^= a3;
	g = Rotate32( g, 19 );
	g = g * 5 + 0xe6546b64;
	f += a4;
	f = Rotate32( f, 19 );
	f = f * 5 + 0xe6546b64;
	size_t iters = ( len - 1 ) / 20;
	do {
		const auto a0 = Rotate32( Fetch32( s ) * c1, 17 ) * c2;
		const auto a1 = Fetch32( s + 4 );
		const auto a2 = Rotate32( Fetch32( s + 8 ) * c1, 17 ) * c2;
		const auto a3 = Rotate32( Fetch32( s + 12 ) * c1, 17 ) * c2;
		const auto a4 = Fetch32( s + 16 );
		h ^= a0;
		h = Rotate32( h, 18 );
		h = h * 5 + 0xe6546b64;
		f += a1;
		f = Rotate32( f, 19 );
		f = f * c1;
		g += a2;
		g = Rotate32( g, 18 );
		g = g * 5 + 0xe6546b64;
		h ^= a3 + a1;
		h = Rotate32( h, 19 );
		h = h * 5 + 0xe6546b64;
		g ^= a4;
		g = bswap_32( g ) * 5;
		h += a4 * 5;
		h = bswap_32( h );
		f += a0;
		PERMUTE3( f, h, g );
		s += 20;
	} while ( --iters != 0 );
	g = Rotate32( g, 11 ) * c1;
	g = Rotate32( g, 17 ) * c1;
	f = Rotate32( f, 11 ) * c1;
	f = Rotate32( f, 17 ) * c1;
	h = Rotate32( h + g, 19 );
	h = h * 5 + 0xe6546b64;
	h = Rotate32( h, 17 ) * c1;
	h = Rotate32( h + f, 19 );
	h = h * 5 + 0xe6546b64;
	h = Rotate32( h, 17 ) * c1;
	return h;
}