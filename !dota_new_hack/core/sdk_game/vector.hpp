#pragma once

#include <cmath>

#define CHECK_VALID( _v ) 0
#define Assert( _exp ) ((void)0)

#define FastSqrt(x)			(sqrt)(x)

inline constexpr auto M_PI = 3.14159265358979323846;

inline constexpr auto M_PI_F = ((float)(M_PI));

inline constexpr auto M_PHI = 1.61803398874989484820;

#ifndef RAD2DEG
#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )
#endif

#ifndef DEG2RAD
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#endif

// MOVEMENT INFO
enum
{
	PITCH = 0,	// up / down
	YAW,		// left / right
	ROLL		// fall over
};

// decls for aligning data

#define DECL_ALIGN(x) __attribute__((aligned(x)))


#define ALIGN16 DECL_ALIGN(16)
#define VALVE_RAND_MAX 0x7fff
#define VectorExpand(v) (v).x, (v).y, (v).z

struct matrix3x4_t
{
	matrix3x4_t( ) {}
	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23 )
	{
		m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
	}

	float* operator[]( int i ) { Assert( (i >= 0) && (i < 3) ); return m_flMatVal[i]; }
	const float* operator[]( int i ) const { Assert( (i >= 0) && (i < 3) ); return m_flMatVal[i]; }
	float* Base( ) { return &m_flMatVal[0][0]; }
	const float* Base( ) const { return &m_flMatVal[0][0]; }

	float m_flMatVal[3][4];
};

class vector3d
{
public:
	float x, y, z;
	vector3d( void );
	vector3d( float X, float Y, float Z );
	void Init( float ix = 0.0f, float iy = 0.0f, float iz = 0.0f );
	bool is_valid( ) const;
	float operator[]( int i ) const;
	float& operator[]( int i );
	inline void Zero( );
	bool operator==( const vector3d& v ) const;
	bool operator!=( const vector3d& v ) const;
	inline vector3d& operator+=( const vector3d& v );
	inline vector3d& operator-=( const vector3d& v );
	inline vector3d& operator*=( const vector3d& v );
	inline vector3d& operator*=( float s );
	inline vector3d& operator/=( const vector3d& v );
	inline vector3d& operator/=( float s );
	inline vector3d& operator+=( float fl );
	inline vector3d& operator-=( float fl );
	inline float	Length( ) const;
	inline float LengthSqr( void ) const
	{
		CHECK_VALID( *this );
		return (x * x + y * y + z * z);
	}
	bool IsZero( float tolerance = 0.01f ) const
	{
		return (x > -tolerance && x < tolerance&&
			y > -tolerance && y < tolerance&&
			z > -tolerance && z < tolerance);
	}
	vector3d	Normalize( );
	float	NormalizeInPlace( );
	inline float	DistTo( const vector3d& vOther ) const;
	inline float	DistToSqr( const vector3d& vOther ) const;
	float	Dot( const vector3d& vOther ) const;
	float	Length2D( void ) const;
	float	Length2DSqr( void ) const;
	vector3d& operator=( const vector3d& vOther );
	vector3d	operator-( void ) const;
	vector3d	operator+( const vector3d& v ) const;
	vector3d	operator-( const vector3d& v ) const;
	vector3d	operator*( const vector3d& v ) const;
	vector3d	operator/( const vector3d& v ) const;
	vector3d	operator*( float fl ) const;
	vector3d	operator/( float fl ) const;
	// Base address...
	float* Base( );
	float const* Base( ) const;
};

//===============================================
inline void vector3d::Init( float ix, float iy, float iz )
{
	x = ix; y = iy; z = iz;
	CHECK_VALID( *this );
}
//===============================================
inline vector3d::vector3d( float X, float Y, float Z )
{
	x = X; y = Y; z = Z;
	CHECK_VALID( *this );
}
//===============================================
inline vector3d::vector3d( void ) { }
//===============================================
inline void vector3d::Zero( )
{
	x = y = z = 0.0f;
}
//===============================================
inline void VectorClear( vector3d& a )
{
	a.x = a.y = a.z = 0.0f;
}
//===============================================
inline vector3d& vector3d::operator=( const vector3d& vOther )
{
	CHECK_VALID( vOther );
	x = vOther.x; y = vOther.y; z = vOther.z;
	return *this;
}
//===============================================
inline float& vector3d::operator[]( int i )
{
	Assert( (i >= 0) && (i < 3) );
	return ((float*)this)[i];
}
//===============================================
inline float vector3d::operator[]( int i ) const
{
	Assert( (i >= 0) && (i < 3) );
	return ((float*)this)[i];
}
//===============================================
inline bool vector3d::operator==( const vector3d& src ) const
{
	CHECK_VALID( src );
	CHECK_VALID( *this );
	return (src.x == x) && (src.y == y) && (src.z == z);
}
//===============================================
inline bool vector3d::operator!=( const vector3d& src ) const
{
	CHECK_VALID( src );
	CHECK_VALID( *this );
	return (src.x != x) || (src.y != y) || (src.z != z);
}
//===============================================
inline void VectorCopy( const vector3d& src, vector3d& dst )
{
	CHECK_VALID( src );
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}
//===============================================
inline  vector3d& vector3d::operator+=( const vector3d& v )
{
	CHECK_VALID( *this );
	CHECK_VALID( v );
	x += v.x; y += v.y; z += v.z;
	return *this;
}
//===============================================
inline  vector3d& vector3d::operator-=( const vector3d& v )
{
	CHECK_VALID( *this );
	CHECK_VALID( v );
	x -= v.x; y -= v.y; z -= v.z;
	return *this;
}
//===============================================
inline  vector3d& vector3d::operator*=( float fl )
{
	x *= fl;
	y *= fl;
	z *= fl;
	CHECK_VALID( *this );
	return *this;
}
//===============================================
inline  vector3d& vector3d::operator*=( const vector3d& v )
{
	CHECK_VALID( v );
	x *= v.x;
	y *= v.y;
	z *= v.z;
	CHECK_VALID( *this );
	return *this;
}
//===============================================
inline vector3d& vector3d::operator+=( float fl )
{
	x += fl;
	y += fl;
	z += fl;
	CHECK_VALID( *this );
	return *this;
}
//===============================================
inline vector3d& vector3d::operator-=( float fl )
{
	x -= fl;
	y -= fl;
	z -= fl;
	CHECK_VALID( *this );
	return *this;
}
//===============================================
inline  vector3d& vector3d::operator/=( float fl )
{
	Assert( fl != 0.0f );
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	CHECK_VALID( *this );
	return *this;
}
//===============================================
inline  vector3d& vector3d::operator/=( const vector3d& v )
{
	CHECK_VALID( v );
	Assert( v.x != 0.0f && v.y != 0.0f && v.z != 0.0f );
	x /= v.x;
	y /= v.y;
	z /= v.z;
	CHECK_VALID( *this );
	return *this;
}
//===============================================
inline float vector3d::Length( void ) const
{
	CHECK_VALID( *this );

	float root = 0.0f;

	float sqsr = x * x + y * y + z * z;

	root = sqrt( sqsr );

	return root;
}
//===============================================
inline float vector3d::Length2D( void ) const
{
	CHECK_VALID( *this );

	float root = 0.0f;

	float sqst = x * x + y * y;

	root = sqrt( sqst );

	return root;
}
//===============================================
inline float vector3d::Length2DSqr( void ) const
{
	return (x * x + y * y);
}
//===============================================
inline vector3d CrossProduct( const vector3d& a, const vector3d& b )
{
	return vector3d( a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x );
}
//===============================================
float vector3d::DistTo( const vector3d& vOther ) const
{
	vector3d delta;

	delta.x = x - vOther.x;
	delta.y = y - vOther.y;
	delta.z = z - vOther.z;

	return delta.Length( );
}
float vector3d::DistToSqr( const vector3d& vOther ) const
{
	vector3d delta;

	delta.x = x - vOther.x;
	delta.y = y - vOther.y;
	delta.z = z - vOther.z;

	return delta.LengthSqr( );
}
//===============================================
inline vector3d vector3d::Normalize( )
{
	vector3d vector;
	float length = this->Length( );

	if ( length != 0 )
	{
		vector.x = x / length;
		vector.y = y / length;
		vector.z = z / length;
	}
	else
	{
		vector.x = vector.y = 0.0f; vector.z = 1.0f;
	}

	return vector;
}
//===============================================
inline float vector3d::NormalizeInPlace( )
{
	vector3d& v = *this;

	float iradius = 1.f / (this->Length( ) + 1.192092896e-07F); //FLT_EPSILON

	v.x *= iradius;
	v.y *= iradius;
	v.z *= iradius;
}
//===============================================
inline float VectorNormalize( vector3d& v )
{
	Assert( v.is_valid( ) );
	float l = v.Length( );
	if ( l != 0.0f )
	{
		v /= l;
	}
	else
	{
		// FIXME:
		// Just copying the existing implemenation; shouldn't res.z == 0?
		v.x = v.y = 0.0f; v.z = 1.0f;
	}
	return l;
}
//===============================================
inline float VectorNormalize( float* v )
{
	return VectorNormalize( *(reinterpret_cast<vector3d*>(v)) );
}
//===============================================
inline vector3d vector3d::operator+( const vector3d& v ) const
{
	vector3d res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;
	return res;
}

//===============================================
inline vector3d vector3d::operator-( const vector3d& v ) const
{
	vector3d res;
	res.x = x - v.x;
	res.y = y - v.y;
	res.z = z - v.z;
	return res;
}
//===============================================
inline vector3d vector3d::operator*( float fl ) const
{
	vector3d res;
	res.x = x * fl;
	res.y = y * fl;
	res.z = z * fl;
	return res;
}
//===============================================
inline vector3d vector3d::operator*( const vector3d& v ) const
{
	vector3d res;
	res.x = x * v.x;
	res.y = y * v.y;
	res.z = z * v.z;
	return res;
}
//===============================================
inline vector3d vector3d::operator/( float fl ) const
{
	vector3d res;
	res.x = x / fl;
	res.y = y / fl;
	res.z = z / fl;
	return res;
}
//===============================================
inline vector3d vector3d::operator/( const vector3d& v ) const
{
	vector3d res;
	res.x = x / v.x;
	res.y = y / v.y;
	res.z = z / v.z;
	return res;
}
inline float vector3d::Dot( const vector3d& vOther ) const
{
	const vector3d& a = *this;

	return(a.x * vOther.x + a.y * vOther.y + a.z * vOther.z);
}

//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------

inline float VectorLength( const vector3d& v )
{
	CHECK_VALID( v );
	return (float)FastSqrt( v.x * v.x + v.y * v.y + v.z * v.z );
}

//VECTOR SUBTRAC
inline void VectorSubtract( const vector3d& a, const vector3d& b, vector3d& c )
{
	CHECK_VALID( a );
	CHECK_VALID( b );
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}

//VECTORADD
inline void VectorAdd( const vector3d& a, const vector3d& b, vector3d& c )
{
	CHECK_VALID( a );
	CHECK_VALID( b );
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
inline float* vector3d::Base( )
{
	return (float*)this;
}

inline float const* vector3d::Base( ) const
{
	return (float const*)this;
}

inline void VectorMAInline( const float* start, float scale, const float* direction, float* dest )
{
	dest[0] = start[0] + direction[0] * scale;
	dest[1] = start[1] + direction[1] * scale;
	dest[2] = start[2] + direction[2] * scale;
}

inline void VectorMAInline( const vector3d& start, float scale, const vector3d& direction, vector3d& dest )
{
	dest.x = start.x + direction.x * scale;
	dest.y = start.y + direction.y * scale;
	dest.z = start.z + direction.z * scale;
}

inline void VectorMA( const vector3d& start, float scale, const vector3d& direction, vector3d& dest )
{
	VectorMAInline( start, scale, direction, dest );
}

inline void VectorMA( const float* start, float scale, const float* direction, float* dest )
{
	VectorMAInline( start, scale, direction, dest );
}


//class ALIGN16 VectorAligned : public Vector
//{
//public:
//	inline VectorAligned(void) {};
//	inline VectorAligned(float X, float Y, float Z)
//	{
//		Init(X, Y, Z);
//	}
//
//#ifdef VECTOR_NO_SLOW_OPERATIONS
//
//private:
//	// No copy constructors allowed if we're in optimal mode
//	VectorAligned(const VectorAligned& vOther);
//	VectorAligned(const Vector &vOther);
//
//#else
//public:
//	explicit VectorAligned(const Vector &vOther)
//	{
//		Init(vOther.x, vOther.y, vOther.z);
//	}
//
//	VectorAligned& operator=(const Vector &vOther)
//	{
//		Init(vOther.x, vOther.y, vOther.z);
//		return *this;
//	}
//
//#endif
//	float w;	// this space is used anyway
//};


inline unsigned long& FloatBits( float& f )
{
	return *reinterpret_cast<unsigned long*>(&f);
}

inline bool IsFinite( float f )
{
	return ((FloatBits( f ) & 0x7F800000) != 0x7F800000);
}

//=========================================================
// 2D Vector2D
//=========================================================

class vector2d
{
public:
	// Members
	float x, y;

	// Construction/destruction
	vector2d( void );
	vector2d( float X, float Y );
	vector2d( const float* pFloat );

	// Initialization
	void Init( float ix = 0.0f, float iy = 0.0f );

	// Got any nasty NAN's?
	bool is_valid( ) const;

	// array access...
	float operator[]( int i ) const;
	float& operator[]( int i );

	// Base address...
	float* Base( );
	float const* Base( ) const;

	// Initialization methods
	void Random( float minVal, float maxVal );

	// equality
	bool operator==( const vector2d& v ) const;
	bool operator!=( const vector2d& v ) const;

	// arithmetic operations
	vector2d& operator+=( const vector2d& v );
	vector2d& operator-=( const vector2d& v );
	vector2d& operator*=( const vector2d& v );
	vector2d& operator*=( float s );
	vector2d& operator/=( const vector2d& v );
	vector2d& operator/=( float s );

	// negate the Vector2D components
	void	Negate( );

	// Get the Vector2D's magnitude.
	float	Length( ) const;

	// Get the Vector2D's magnitude squared.
	float	LengthSqr( void ) const;

	// return true if this vector is (0,0) within tolerance
	bool IsZero( float tolerance = 0.01f ) const
	{
		return (x > -tolerance && x < tolerance&&
			y > -tolerance && y < tolerance);
	}

	float	Normalize( );

	// Normalize in place and return the old length.
	float	NormalizeInPlace( );

	// Compare length.
	bool	IsLengthGreaterThan( float val ) const;
	bool	IsLengthLessThan( float val ) const;

	// Get the distance from this Vector2D to the other one.
	float	DistTo( const vector2d& vOther ) const;

	// Get the distance from this Vector2D to the other one squared.
	float	DistToSqr( const vector2d& vOther ) const;

	// Copy
	void	CopyToArray( float* rgfl ) const;

	// Multiply, add, and assign to this (ie: *this = a + b * scalar). This
	// is about 12% faster than the actual Vector2D equation (because it's done per-component
	// rather than per-Vector2D).
	void	MulAdd( const vector2d& a, const vector2d& b, float scalar );

	// Dot product.
	float	Dot( const vector2d& vOther ) const;

	// assignment
	vector2d& operator=( const vector2d& vOther );

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// copy constructors
	vector2d( const vector2d& vOther );

	// arithmetic operations
	vector2d	operator-( void ) const;

	vector2d	operator+( const vector2d& v ) const;
	vector2d	operator-( const vector2d& v ) const;
	vector2d	operator*( const vector2d& v ) const;
	vector2d	operator/( const vector2d& v ) const;
	vector2d	operator*( float fl ) const;
	vector2d	operator/( float fl ) const;

	// Cross product between two vectors.
	vector2d	Cross( const vector2d& vOther ) const;

	// Returns a Vector2D with the min or max in X, Y, and Z.
	vector2d	Min( const vector2d& vOther ) const;
	vector2d	Max( const vector2d& vOther ) const;

#else

private:
	// No copy constructors allowed if we're in optimal mode
	Vector2D( const Vector2D& vOther );
#endif
};

//-----------------------------------------------------------------------------

const vector2d vec2_origin( 0, 0 );
const vector2d vec2_invalid( 3.40282347E+32F, 3.40282347E+32F );

//-----------------------------------------------------------------------------
// Vector2D related operations
//-----------------------------------------------------------------------------

// Vector2D clear
void Vector2DClear( vector2d& a );

// Copy
void Vector2DCopy( const vector2d& src, vector2d& dst );

// Vector2D arithmetic
void Vector2DAdd( const vector2d& a, const vector2d& b, vector2d& result );
void Vector2DSubtract( const vector2d& a, const vector2d& b, vector2d& result );
void Vector2DMultiply( const vector2d& a, float b, vector2d& result );
void Vector2DMultiply( const vector2d& a, const vector2d& b, vector2d& result );
void Vector2DDivide( const vector2d& a, float b, vector2d& result );
void Vector2DDivide( const vector2d& a, const vector2d& b, vector2d& result );
void Vector2DMA( const vector2d& start, float s, const vector2d& dir, vector2d& result );

// Store the min or max of each of x, y, and z into the result.
void Vector2DMin( const vector2d& a, const vector2d& b, vector2d& result );
void Vector2DMax( const vector2d& a, const vector2d& b, vector2d& result );

#define Vector2DExpand( v ) (v).x, (v).y

// Normalization
float Vector2DNormalize( vector2d& v );

// Length
float Vector2DLength( const vector2d& v );

// Dot Product
float DotProduct2D( const vector2d& a, const vector2d& b );

// Linearly interpolate between two vectors
void Vector2DLerp( const vector2d& src1, const vector2d& src2, float t, vector2d& dest );


//-----------------------------------------------------------------------------
//
// Inlined Vector2D methods
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// constructors
//-----------------------------------------------------------------------------

inline vector2d::vector2d( void )
{
#ifdef _DEBUG
	// Initialize to NAN to catch errors
	//x = y = float_NAN;
#endif
}

inline vector2d::vector2d( float X, float Y )
{
	x = X; y = Y;
	Assert( is_valid( ) );
}

inline vector2d::vector2d( const float* pFloat )
{
	Assert( pFloat );
	x = pFloat[0]; y = pFloat[1];
	Assert( is_valid( ) );
}


//-----------------------------------------------------------------------------
// copy constructor
//-----------------------------------------------------------------------------

inline vector2d::vector2d( const vector2d& vOther )
{
	Assert( vOther.is_valid( ) );
	x = vOther.x; y = vOther.y;
}

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------

inline void vector2d::Init( float ix, float iy )
{
	x = ix; y = iy;
	Assert( is_valid( ) );
}

inline void vector2d::Random( float minVal, float maxVal )
{
	x = minVal + ((float)rand( ) / VALVE_RAND_MAX) * (maxVal - minVal);
	y = minVal + ((float)rand( ) / VALVE_RAND_MAX) * (maxVal - minVal);
}

inline void Vector2DClear( vector2d& a )
{
	a.x = a.y = 0.0f;
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------

inline vector2d& vector2d::operator=( const vector2d& vOther )
{
	Assert( vOther.is_valid( ) );
	x = vOther.x; y = vOther.y;
	return *this;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------

inline float& vector2d::operator[]( int i )
{
	Assert( (i >= 0) && (i < 2) );
	return ((float*)this)[i];
}

inline float vector2d::operator[]( int i ) const
{
	Assert( (i >= 0) && (i < 2) );
	return ((float*)this)[i];
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------

inline float* vector2d::Base( )
{
	return (float*)this;
}

inline float const* vector2d::Base( ) const
{
	return (float const*)this;
}

//-----------------------------------------------------------------------------
// IsValid?
//-----------------------------------------------------------------------------

inline bool vector2d::is_valid( ) const
{
	return IsFinite( x ) && IsFinite( y );
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------

inline bool vector2d::operator==( const vector2d& src ) const
{
	Assert( src.is_valid( ) && is_valid( ) );
	return (src.x == x) && (src.y == y);
}

inline bool vector2d::operator!=( const vector2d& src ) const
{
	Assert( src.is_valid( ) && is_valid( ) );
	return (src.x != x) || (src.y != y);
}


//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------

inline void Vector2DCopy( const vector2d& src, vector2d& dst )
{
	Assert( src.is_valid( ) );
	dst.x = src.x;
	dst.y = src.y;
}

inline void	vector2d::CopyToArray( float* rgfl ) const
{
	Assert( is_valid( ) );
	Assert( rgfl );
	rgfl[0] = x; rgfl[1] = y;
}

//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------

inline void vector2d::Negate( )
{
	Assert( is_valid( ) );
	x = -x; y = -y;
}

inline vector2d& vector2d::operator+=( const vector2d& v )
{
	Assert( is_valid( ) && v.is_valid( ) );
	x += v.x; y += v.y;
	return *this;
}

inline vector2d& vector2d::operator-=( const vector2d& v )
{
	Assert( is_valid( ) && v.is_valid( ) );
	x -= v.x; y -= v.y;
	return *this;
}

inline vector2d& vector2d::operator*=( float fl )
{
	x *= fl;
	y *= fl;
	Assert( is_valid( ) );
	return *this;
}

inline vector2d& vector2d::operator*=( const vector2d& v )
{
	x *= v.x;
	y *= v.y;
	Assert( is_valid( ) );
	return *this;
}

inline vector2d& vector2d::operator/=( float fl )
{
	Assert( fl != 0.0f );
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	Assert( is_valid( ) );
	return *this;
}

inline vector2d& vector2d::operator/=( const vector2d& v )
{
	Assert( v.x != 0.0f && v.y != 0.0f );
	x /= v.x;
	y /= v.y;
	Assert( is_valid( ) );
	return *this;
}

inline void Vector2DAdd( const vector2d& a, const vector2d& b, vector2d& c )
{
	Assert( a.is_valid( ) && b.is_valid( ) );
	c.x = a.x + b.x;
	c.y = a.y + b.y;
}

inline void Vector2DSubtract( const vector2d& a, const vector2d& b, vector2d& c )
{
	Assert( a.is_valid( ) && b.is_valid( ) );
	c.x = a.x - b.x;
	c.y = a.y - b.y;
}

inline void Vector2DMultiply( const vector2d& a, float b, vector2d& c )
{
	Assert( a.is_valid( ) && IsFinite( b ) );
	c.x = a.x * b;
	c.y = a.y * b;
}

inline void Vector2DMultiply( const vector2d& a, const vector2d& b, vector2d& c )
{
	Assert( a.is_valid( ) && b.is_valid( ) );
	c.x = a.x * b.x;
	c.y = a.y * b.y;
}


inline void Vector2DDivide( const vector2d& a, float b, vector2d& c )
{
	Assert( a.is_valid( ) );
	Assert( b != 0.0f );
	float oob = 1.0f / b;
	c.x = a.x * oob;
	c.y = a.y * oob;
}

inline void Vector2DDivide( const vector2d& a, const vector2d& b, vector2d& c )
{
	Assert( a.is_valid( ) );
	Assert( (b.x != 0.0f) && (b.y != 0.0f) );
	c.x = a.x / b.x;
	c.y = a.y / b.y;
}

inline void Vector2DMA( const vector2d& start, float s, const vector2d& dir, vector2d& result )
{
	Assert( start.is_valid( ) && IsFinite( s ) && dir.is_valid( ) );
	result.x = start.x + s * dir.x;
	result.y = start.y + s * dir.y;
}

// FIXME: Remove
// For backwards compatability
inline void	vector2d::MulAdd( const vector2d& a, const vector2d& b, float scalar )
{
	x = a.x + b.x * scalar;
	y = a.y + b.y * scalar;
}

inline void Vector2DLerp( const vector2d& src1, const vector2d& src2, float t, vector2d& dest )
{
	dest[0] = src1[0] + (src2[0] - src1[0]) * t;
	dest[1] = src1[1] + (src2[1] - src1[1]) * t;
}

//-----------------------------------------------------------------------------
// dot, cross
//-----------------------------------------------------------------------------
inline float DotProduct2D( const vector2d& a, const vector2d& b )
{
	Assert( a.is_valid( ) && b.is_valid( ) );
	return(a.x * b.x + a.y * b.y);
}

// for backwards compatability
inline float vector2d::Dot( const vector2d& vOther ) const
{
	return DotProduct2D( *this, vOther );
}


//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------
inline float Vector2DLength( const vector2d& v )
{
	Assert( v.is_valid( ) );
	return (float)FastSqrt( v.x * v.x + v.y * v.y );
}

inline float vector2d::LengthSqr( void ) const
{
	Assert( is_valid( ) );
	return (x * x + y * y);
}

inline float vector2d::NormalizeInPlace( )
{
	return Vector2DNormalize( *this );
}

inline bool vector2d::IsLengthGreaterThan( float val ) const
{
	return LengthSqr( ) > val * val;
}

inline bool vector2d::IsLengthLessThan( float val ) const
{
	return LengthSqr( ) < val * val;
}

inline float vector2d::Length( void ) const
{
	return Vector2DLength( *this );
}


inline void Vector2DMin( const vector2d& a, const vector2d& b, vector2d& result )
{
	result.x = (a.x < b.x) ? a.x : b.x;
	result.y = (a.y < b.y) ? a.y : b.y;
}


inline void Vector2DMax( const vector2d& a, const vector2d& b, vector2d& result )
{
	result.x = (a.x > b.x) ? a.x : b.x;
	result.y = (a.y > b.y) ? a.y : b.y;
}


//-----------------------------------------------------------------------------
// Normalization
//-----------------------------------------------------------------------------
inline float Vector2DNormalize( vector2d& v )
{
	Assert( v.is_valid( ) );
	float l = v.Length( );
	if ( l != 0.0f )
	{
		v /= l;
	}
	else
	{
		v.x = v.y = 0.0f;
	}
	return l;
}


//-----------------------------------------------------------------------------
// Get the distance from this Vector2D to the other one
//-----------------------------------------------------------------------------
inline float vector2d::DistTo( const vector2d& vOther ) const
{
	vector2d delta;
	Vector2DSubtract( *this, vOther, delta );
	return delta.Length( );
}

inline float vector2d::DistToSqr( const vector2d& vOther ) const
{
	vector2d delta;
	Vector2DSubtract( *this, vOther, delta );
	return delta.LengthSqr( );
}


//-----------------------------------------------------------------------------
// Computes the closest point to vecTarget no farther than flMaxDist from vecStart
//-----------------------------------------------------------------------------
inline void ComputeClosestPoint2D( const vector2d& vecStart, float flMaxDist, const vector2d& vecTarget, vector2d* pResult )
{
	vector2d vecDelta;
	Vector2DSubtract( vecTarget, vecStart, vecDelta );
	float flDistSqr = vecDelta.LengthSqr( );
	if ( flDistSqr <= flMaxDist * flMaxDist )
	{
		*pResult = vecTarget;
	}
	else
	{
		vecDelta /= FastSqrt( flDistSqr );
		Vector2DMA( vecStart, flMaxDist, vecDelta, *pResult );
	}
}



//-----------------------------------------------------------------------------
//
// Slow methods
//
//-----------------------------------------------------------------------------

#ifndef VECTOR_NO_SLOW_OPERATIONS
#endif
//-----------------------------------------------------------------------------
// Returns a Vector2D with the min or max in X, Y, and Z.
//-----------------------------------------------------------------------------

inline vector2d vector2d::Min( const vector2d& vOther ) const
{
	return vector2d( x < vOther.x ? x : vOther.x,
		y < vOther.y ? y : vOther.y );
}

inline vector2d vector2d::Max( const vector2d& vOther ) const
{
	return vector2d( x > vOther.x ? x : vOther.x,
		y > vOther.y ? y : vOther.y );
}


//-----------------------------------------------------------------------------
// arithmetic operations
//-----------------------------------------------------------------------------

inline vector2d vector2d::operator-( void ) const
{
	return vector2d( -x, -y );
}

inline vector2d vector2d::operator+( const vector2d& v ) const
{
	vector2d res;
	Vector2DAdd( *this, v, res );
	return res;
}

inline vector2d vector2d::operator-( const vector2d& v ) const
{
	vector2d res;
	Vector2DSubtract( *this, v, res );
	return res;
}

inline vector2d vector2d::operator*( float fl ) const
{
	vector2d res;
	Vector2DMultiply( *this, fl, res );
	return res;
}

inline vector2d vector2d::operator*( const vector2d& v ) const
{
	vector2d res;
	Vector2DMultiply( *this, v, res );
	return res;
}

inline vector2d vector2d::operator/( float fl ) const
{
	vector2d res;
	Vector2DDivide( *this, fl, res );
	return res;
}

inline vector2d vector2d::operator/( const vector2d& v ) const
{
	vector2d res;
	Vector2DDivide( *this, v, res );
	return res;
}

inline vector2d operator*( float fl, const vector2d& v )
{
	return v * fl;
}

inline double deg2rad( double deg ) {
	return deg * M_PI / 180.0;
}

class QAngleByValue;
class QAngle
{
public:

	float pitch_deg{};
	float yaw_deg{};
	float roll_deg{};
public:
	double GetPitch( ) const noexcept
	{
		return deg2rad( pitch_deg );
	}

	double GetYaw( ) const noexcept
	{
		return deg2rad( yaw_deg );
	}

	double GetRoll( ) const noexcept
	{
		return deg2rad( roll_deg );
	}
public:
	vector3d GetForward( ) const noexcept
	{
		vector3d result{};
		const auto pitch = GetPitch( );
		const auto yaw = GetYaw( );
		const auto cp = std::cosf( pitch );
		const auto sp = std::sinf( pitch );
		const auto cy = std::cosf( yaw );
		const auto sy = std::sinf( yaw );
		return { cp * cy, cp * sy, -sp };
	}

	// Members
	float x, y, z;

	// Construction/destruction
	QAngle( void );
	QAngle( float X, float Y, float Z );
	//      QAngle(RadianEuler const &angles);      // evil auto type promotion!!!

	// Allow pass-by-value
	operator QAngleByValue& () { return *((QAngleByValue*)(this)); }
	operator const QAngleByValue& () const { return *((const QAngleByValue*)(this)); }

	// Initialization
	void Init( float ix = 0.0f, float iy = 0.0f, float iz = 0.0f );
	void Random( float minVal, float maxVal );

	// Got any nasty NAN's?
	bool is_valid( ) const;
	void invalidate( );

	// array access...
	float operator[]( int i ) const;
	float& operator[]( int i );

	// Base address...
	float* Base( );
	float const* Base( ) const;

	// equality
	bool operator==( const QAngle& v ) const;
	bool operator!=( const QAngle& v ) const;

	bool IsZero( float tolerance = 0.01f ) const
	{
		return (x > -tolerance && x < tolerance&&
			y > -tolerance && y < tolerance&&
			z > -tolerance && z < tolerance);
	}

	// arithmetic operations
	QAngle& operator+=( const QAngle& v );
	QAngle& operator-=( const QAngle& v );
	QAngle& operator*=( float s );
	QAngle& operator/=( float s );

	// Get the vector's magnitude.
	float   Length( ) const;
	float   LengthSqr( ) const;

	// negate the QAngle components
	//void  Negate();

	// No assignment operators either...
	QAngle& operator=( const QAngle& src );

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// copy constructors

	// arithmetic operations
	QAngle  operator-( void ) const;

	QAngle  operator+( const QAngle& v ) const;
	QAngle  operator-( const QAngle& v ) const;
	QAngle  operator*( float fl ) const;
	QAngle  operator/( float fl ) const;
#else

private:
	// No copy constructors allowed if we're in optimal mode
	QAngle( const QAngle& vOther );

#endif
};

//-----------------------------------------------------------------------------
// constructors
//-----------------------------------------------------------------------------
inline QAngle::QAngle( void )
{
#ifdef _DEBUG
#ifdef VECTOR_PARANOIA
	// Initialize to NAN to catch errors
	x = y = z = VEC_T_NAN;
#endif
#endif
}

inline QAngle::QAngle( float X, float Y, float Z )
{
	x = X; y = Y; z = Z;
	CHECK_VALID( *this );
}

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------
inline void QAngle::Init( float ix, float iy, float iz )
{
	x = ix; y = iy; z = iz;
	CHECK_VALID( *this );
}

inline void QAngle::Random( float minVal, float maxVal )
{
	x = minVal + ((float)rand( ) / RAND_MAX) * (maxVal - minVal);
	y = minVal + ((float)rand( ) / RAND_MAX) * (maxVal - minVal);
	z = minVal + ((float)rand( ) / RAND_MAX) * (maxVal - minVal);
	CHECK_VALID( *this );
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------
inline QAngle& QAngle::operator=( const QAngle& vOther )
{
	CHECK_VALID( vOther );
	x = vOther.x; y = vOther.y; z = vOther.z;
	return *this;
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------
inline bool QAngle::operator==( const QAngle& src ) const
{
	CHECK_VALID( src );
	CHECK_VALID( *this );
	return (src.x == x) && (src.y == y) && (src.z == z);
}

inline bool QAngle::operator!=( const QAngle& src ) const
{
	CHECK_VALID( src );
	CHECK_VALID( *this );
	return (src.x != x) || (src.y != y) || (src.z != z);
}

//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------
inline QAngle& QAngle::operator+=( const QAngle& v )
{
	CHECK_VALID( *this );
	CHECK_VALID( v );
	x += v.x; y += v.y; z += v.z;
	return *this;
}

inline QAngle& QAngle::operator-=( const QAngle& v )
{
	CHECK_VALID( *this );
	CHECK_VALID( v );
	x -= v.x; y -= v.y; z -= v.z;
	return *this;
}

inline QAngle& QAngle::operator*=( float fl )
{
	x *= fl;
	y *= fl;
	z *= fl;
	CHECK_VALID( *this );
	return *this;
}

inline QAngle& QAngle::operator/=( float fl )
{
	Assert( fl != 0.0f );
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	CHECK_VALID( *this );
	return *this;
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
inline float* QAngle::Base( )
{
	return (float*)this;
}

inline float const* QAngle::Base( ) const
{
	return (float const*)this;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline float& QAngle::operator[]( int i )
{
	Assert( (i >= 0) && (i < 3) );
	return ((float*)this)[i];
}

inline float QAngle::operator[]( int i ) const
{
	Assert( (i >= 0) && (i < 3) );
	return ((float*)this)[i];
}

//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------
inline float QAngle::Length( ) const
{
	CHECK_VALID( *this );
	return (float)FastSqrt( LengthSqr( ) );
}


inline float QAngle::LengthSqr( ) const
{
	CHECK_VALID( *this );
	return x * x + y * y + z * z;
}


//-----------------------------------------------------------------------------
// arithmetic operations (SLOW!!)
//-----------------------------------------------------------------------------
#ifndef VECTOR_NO_SLOW_OPERATIONS

inline QAngle QAngle::operator-( void ) const
{
	return QAngle( -x, -y, -z );
}

inline QAngle QAngle::operator+( const QAngle& v ) const
{
	QAngle res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;
	return res;
}

inline QAngle QAngle::operator-( const QAngle& v ) const
{
	QAngle res;
	res.x = x - v.x;
	res.y = y - v.y;
	res.z = z - v.z;
	return res;
}

inline QAngle QAngle::operator*( float fl ) const
{
	QAngle res;
	res.x = x * fl;
	res.y = y * fl;
	res.z = z * fl;
	return res;
}

inline QAngle QAngle::operator/( float fl ) const
{
	QAngle res;
	res.x = x / fl;
	res.y = y / fl;
	res.z = z / fl;
	return res;
}

inline QAngle operator*( float fl, const QAngle& v )
{
	return v * fl;
}

#endif // VECTOR_NO_SLOW_OPERATIONS


//QANGLE SUBTRAC
inline void QAngleSubtract( const QAngle& a, const QAngle& b, QAngle& c )
{
	CHECK_VALID( a );
	CHECK_VALID( b );
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}

//QANGLEADD
inline void QAngleAdd( const QAngle& a, const QAngle& b, QAngle& c )
{
	CHECK_VALID( a );
	CHECK_VALID( b );
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
}