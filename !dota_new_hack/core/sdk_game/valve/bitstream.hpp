#pragma once
#undef min

#include <cstdint>

#include <intrin.h>

#ifdef _DEBUG
#define proto_string(st) *(const char**)&st
#else
#define proto_string(st) st
#endif

#ifdef _DEBUG
#define proto_string_np(st) (const char*)&st
#else
#define proto_string_np(st) st
#endif

#ifdef _DEBUG
#define proto_string_p(st) *(const char**)&st
#else
#define proto_string_p(st) st.c_str()
#endif


// pasted from https://github.com/ButterflyStats/butterfly
#define force_inline FORCEINLINE
#define VARINT32_MAX 5  /// Maximum number of bytes to read for a 32 bit varint
#define VARINT64_MAX 10 /// Maximum number of bytes to read for a 64 bit varint

static constexpr uint64_t masks[ 64 ] = {
       0x0,             0x1,              0x3,              0x7,
       0xf,             0x1f,             0x3f,             0x7f,
       0xff,            0x1ff,            0x3ff,            0x7ff,
       0xfff,           0x1fff,           0x3fff,           0x7fff,
       0xffff,          0x1ffff,          0x3ffff,          0x7ffff,
       0xfffff,         0x1fffff,         0x3fffff,         0x7fffff,
       0xffffff,        0x1ffffff,        0x3ffffff,        0x7ffffff,
       0xfffffff,       0x1fffffff,       0x3fffffff,       0x7fffffff,
       0xffffffff,      0x1ffffffff,      0x3ffffffff,      0x7ffffffff,
       0xfffffffff,     0x1fffffffff,     0x3fffffffff,     0x7fffffffff,
       0xffffffffff,    0x1ffffffffff,    0x3ffffffffff,    0x7ffffffffff,
       0xfffffffffff,   0x1fffffffffff,   0x3fffffffffff,   0x7fffffffffff,
       0xffffffffffff,  0x1ffffffffffff,  0x3ffffffffffff,  0x7ffffffffffff,
       0xfffffffffffff, 0x1fffffffffffff, 0x3fffffffffffff, 0x7fffffffffffff
};
/* clang-format on */

/** Return int with bit set at given position */
static constexpr uint64_t bit_at( uint8_t bit ) { return ( 1 << bit ); }

/** Read-Only bitstream implementation */
class bitstream {
public:
    /** Type used to keep track of the stream position */
    typedef uint64_t size_type;

    /** Creates an empty bitstream */
    bitstream( ) : data{}, pos{ 0 }, size{ 0 }, owns{ false } {}

    /** Creates a bitstream from an existing buffer */
    bitstream( const std::string& str ) : pos{ 0 }, size{ str.size( ) << 3 }, owns{ true } {
        // Check size requirements
        //ASSERT_LESS( size, 0xffffffff, "Bitstream to large" );

        // Reserve the memory in beforehand so we can just memcpy everything
        data = new uint32_t[ ( str.size( ) + 3 ) / 4 + 1 ];
        memcpy( &data[ 0 ], str.c_str( ), str.size( ) );
    }

    /** Copy-Constructor */
    bitstream( const bitstream& b ) : data( nullptr ), pos( b.pos ), size( b.size ), owns( true ) {
        data = new uint32_t[ ( ( size >> 3 ) + 3 ) / 4 + 1 ];
        memcpy( &data[ 0 ], &b.data[ 0 ], ( size >> 3 ) );
    }

    /** Move-Constructor */
    bitstream( bitstream&& b ) : data( b.data ), pos( b.pos ), size( b.size ) {
        b.data = nullptr;
        b.pos = 0;
        b.size = 0;
    }

    /** Destructor */
    ~bitstream( ) {
        if ( owns )
            delete[] data;
    }

    /** Assignment operator */
    bitstream& operator=( bitstream t ) {
        swap( t );
        return *this;
    }

    /** Swap this bitstream with given one */
    void swap( bitstream& b ) {
        std::swap( data, b.data );
        std::swap( pos, b.pos );
        std::swap( size, b.size );
        std::swap( owns, b.owns );
    }

    /** Checkes whether there is still data left to be read. */
    inline bool good( ) const { return pos < size; }

    /** Returns number of bits left */
    inline size_type remaining( ) const { return size - pos; }

    /** Returns the size of the stream in bits. */
    inline size_type end( ) const { return size; }

    /** Returns the current position of the stream in bits. */
    inline size_type position( ) const { return pos; }

    /** Sets bitstream position */
    inline void setPosition( const size_type s ) { pos = s; }

    /**
     * Returns result of reading n bits into an uint32_t.
     *
     * This function can read a maximum of 32 bits at once. If the amount of data requested
     * exceeds the remaining size of the current chunk it wraps around to the next one.
     */
    force_inline uint32_t read( const size_type n ) {
        // make sure the data read fits the return type
       // ASSERT_LESS( n, size - pos, "Bitstream overflow" );
       // ASSERT_LESS( n, 33, "Trying to read more than 32 bits" );

        static constexpr uint32_t bitSize = sizeof( uint32_t ) << 3; // size of chunk in bits
        const uint32_t start = pos >> 5;                // current active chunk
        const uint32_t end = ( pos + n - 1 ) >> 5;    // next chunk if data needs to be wrapped
        const uint32_t shift = ( pos & 31 );            // shift amount

        pos += n;
        return ( ( data[ start ] >> shift ) | ( data[ end ] << ( bitSize - shift ) ) ) & masks[ n ];
    }

    /**
     * Seek n bits forward.
     *
     * If the resulting position would overflow, it is set to the maximum one possible.
     */
    void seekForward( const size_type n ) {
        pos += n;

        if ( pos > size )
            pos = size;
    }

    /**
     * Seek n bits backward.
     *
     * If the resulting position would underflow, it is set to 0.
     */
    void seekBackward( const size_type n ) {
        if ( n > pos ) {
            pos = 0;
        }
        else {
            pos -= n;
        }
    }

    /** Reads a boolean */
    bool readBool( ) {
        bool ret = ( data[ pos >> 5 ] & bit_at( pos & 31 ) );
        pos += 1;

        return ret;
    }

    /** Reads a variable sized uint32_t from the stream. */
    uint32_t readVarUInt32( ) {
        uint32_t readCount = 0;
        uint32_t value = 0;

        char tmpBuf;
        do {
            if ( readCount == VARINT32_MAX ) // return when maximum number of iterations is reached
                return value;

            tmpBuf = (char)read( 8 );
            value |= (uint32_t)( tmpBuf & 0x7F ) << ( 7 * readCount );
            ++readCount;
        } while ( tmpBuf & 0x80 );

        return value;
    }

    /** Reads a variable sized uint64_t from the stream. */
    uint64_t readVarUInt64( ) {
        uint32_t readCount = 0;
        uint64_t value = 0;

        uint32_t tmpBuf;
        do {
            if ( readCount == VARINT64_MAX )
                return value;

            tmpBuf = read( 8 );
            value |= static_cast<uint64_t>( tmpBuf & 0x7F ) << ( 7 * readCount );
            ++readCount;
        } while ( tmpBuf & 0x80 );

        return value;
    }

    /**
     * Reads a variable sized int32_t from the stream.
     *
     * The signed version of the varint uses protobuf's zigzag encoding for
     * the sign part.
     */
    int32_t readVarSInt32( ) {
        const uint32_t value = readVarUInt32( );
        return ( value >> 1 ) ^ -static_cast<int32_t>( value & 1 );
    }

    /** Reads a variable sized int64_t from the stream. */
    int64_t readVarSInt64( ) {
        const uint64_t value = readVarUInt64( );
        return ( value >> 1 ) ^ -static_cast<int64_t>( value & 1 );
    }

    /**
     * Reads a null-terminated string into the buffer, stops once it reaches \0 or n chars.
     *
     * n is treated as the number of bytes to be read.
     * n can be arbitrarily large in this context. The underlying read method throws in case an overflow
     * happens.
     */
    void readString( char* buffer, const size_type n ) {
        if ( ( pos & 7 ) == 0 ) {
            uint8_t* cbuf = (uint8_t*)&data[ 0 ] + ( pos >> 3 );

            for ( std::size_t i = 0; i < n; ++i ) {
                buffer[ i ] = ( cbuf[ i ] );

                if ( buffer[ i ] == '\0' ) {
                    pos += i * 8 + 8;
                    break;
                }
            }
        }
        else {
            for ( std::size_t i = 0; i < n; ++i ) {
                buffer[ i ] = static_cast<char>( read( 8 ) );

                if ( buffer[ i ] == '\0' )
                    break;
            }
        }
    }

    /**
     * Reads the exact number of bits into the buffer.
     *
     * The function reads in chunks of 8 bit until n is smaller than that
     * and appends the left over bits
     */
    void readBits( char* buffer, const size_type n ) {
        size_type remaining = n;
        size_type i = 0;

        while ( remaining >= 8 ) {
            buffer[ i++ ] = read( 8 );
            remaining -= 8;
        }

        if ( remaining > 0 )
            buffer[ i++ ] = read( remaining );
    }

    /** Reads exact number of bytes */
    void readBytes( char* buffer, const size_type n ) {
        // optimization if byte aligned
        if ( ( pos & 7 ) == 0 ) {
            uint8_t* cbuf = (uint8_t*)&data[ 0 ];
            memcpy( buffer, &cbuf[ ( pos >> 3 ) ], n );
            pos += n * 8;

            return;
        }

        for ( uint32_t i = 0; i < n; ++i ) {
            buffer[ i ] = read( 8 );
        }
    }

    /** Reads a ubitvar, valve's own variable-length integer encoding. */
    uint32_t readUBitVar( ) {
        uint32_t nId = read( 6 );

        switch ( nId & 0x30 ) {
        case 16:
            nId = ( nId & 15 ) | ( read( 4 ) << 4 );
            break;
        case 32:
            nId = ( nId & 15 ) | ( read( 8 ) << 4 );
            break;
        case 48:
            nId = ( nId & 15 ) | ( read( 28 ) << 4 );
            break;
        }

        return nId;
    }

    /** Reads a fieldpath varint */
    int32_t readFPBitVar( ) {
        if ( readBool( ) )
            return read( 2 );
        if ( readBool( ) )
            return read( 4 );
        if ( readBool( ) )
            return read( 10 );
        if ( readBool( ) )
            return read( 17 );
        return read( 31 );
    }

    /** Reads coord */
    float readCoord( ) {
        float val = 0;

        uint32_t intval = readBool( );
        uint32_t fractval = readBool( );
        bool signbit = false;

        if ( intval != 0 || fractval != 0 ) {
            signbit = readBool( );

            if ( intval )
                intval = read( 14 ) + 1;

            if ( fractval )
                fractval = read( 5 );

            val = intval + static_cast<float>( fractval ) * ( 1.0f / ( 1 << 5 ) );
        }

        if ( signbit )
            return -val;
        else
            return val;
    }
    
    /** Reads an angle */
    float readAngle( uint32_t n ) {
        return static_cast<float>( read( n ) ) * 360.0f / static_cast<float>( 1 << n );
    }

    /** Reads a normalized float */
    float readNormal( ) {
        bool signbit = readBool( );
        uint16_t len = read( 11 );
        float ret = static_cast<float>( len ) * ( 1.0f / static_cast<float>( 1 << 11 ) - 1.0f );

        if ( signbit )
            return -ret;
        else
            return ret;
    }
    
    /** Reads a normalized vector */
    std::array<float, 3> read3BitNormal( ) {
        std::array<float, 3> ret{{0.0f, 0.0f, 0.0f}};

        bool hasX = readBool( );
        bool hasY = readBool( );

        if ( hasX )
            ret[ 0 ] = readNormal( );
        if ( hasY )
            ret[ 1 ] = readNormal( );

        bool negZ = readBool( );
        float prodsum = ret[ 0 ] * ret[ 0 ] + ret[ 1 ] * ret[ 1 ];

        if ( prodsum < 1.0f )
            ret[ 2 ] = sqrt( 1.0 - prodsum );
        else
            ret[ 2 ] = 0.0f;

        if ( negZ )
            ret[ 2 ] = -ret[ 2 ];

        return ret;
    }

private:
    /** Data to read from */
    uint32_t* data;
    /** Current position in the vector in bits */
    size_type pos;
    /** Overall size of the data in bits */
    size_type size;
    /** Whether we own the data memory */
    bool owns;
};

template<typename T = void, typename TT = T> T* GetPointer( TT* data, size_t pos ) {
    return (T*)( (byte*)data + pos );
}
struct custom_stream {
    FORCEINLINE custom_stream( std::string_view view ) : data( view.data( ) ), size_( view.size( ) ), pos( 0 ) {}
    FORCEINLINE custom_stream( const void* data, size_t size ) : data( data ), size_( size ), pos( 0 ) {}
    FORCEINLINE custom_stream( ) : data( nullptr ), size_( 0 ), pos( 0 ) {}

    FORCEINLINE bool good( ) { return this->pos < this->size_; }
    FORCEINLINE void invalidate( ) { this->pos = this->size_; }
    FORCEINLINE size_t position( ) { return this->pos; }
    FORCEINLINE void set_position( size_t pos ) { this->pos = pos; }
    FORCEINLINE size_t size( ) { return this->size_; }
    FORCEINLINE size_t remaining( ) {
        return this->good( )
            ? this->size_ - this->pos
            : 0;
    }
    template<typename T> FORCEINLINE bool read( T& out ) {
        if ( sizeof( T ) > this->remaining( ) ) {
            this->invalidate( );
            return false;
        }
        out = *GetPointer<T>( this->data, this->pos );
        this->pos += sizeof( T );
        return true;
    }
    template<> FORCEINLINE bool read( bool& out ) {
        if ( !this->good( ) )
            return false;
        out = *GetPointer<uint8_t>( this->data, this->pos ) != 0;
        this->pos++;
        return true;
    }
    template<typename T> FORCEINLINE bool readUint64( T& out ) {
        uint64_t ret = 0;
        if ( !this->read( ret ) )
            return false;
        out = ret;
        return true;
    }
    FORCEINLINE bool readBytes( void* dst, size_t size ) {
        if ( size > this->remaining( ) ) {
            this->invalidate( );
            return false;
        }
        memcpy( dst, GetPointer( this->data, this->pos ), size );
        this->pos += size;
        return true;
    }
    FORCEINLINE bool readBytesNoCopy( void*& out, size_t size ) {
        out = GetPointer( this->data, this->pos );
        if ( size > this->remaining( ) ) {
            this->invalidate( );
            return false;
        }
        this->pos += size;
        return true;
    }
    FORCEINLINE bool readNullTerminatedString( std::string_view& out ) {
        auto start = GetPointer<const char>( this->data, this->pos );
        size_t size = 0;
        while ( true ) {
            uint8_t c = 0;
            if ( !this->read( c ) )
                return false;
            if ( c == 0 )
                break;
            size++;
        }
        out = { start, size };
        return true;
    }
    FORCEINLINE bool readOffsetString( std::string_view& out ) {
        uint32_t offset = 0;
        if ( !this->read( offset ) )
            return false;
        if ( offset == 0 ) {
            out = "";
            return true;
        }
        auto saved_pos = this->position( );
        this->set_position( saved_pos - 4 + offset );
        auto res = this->readNullTerminatedString( out );
        this->set_position( saved_pos );
        return res;
    }
    template<typename T> FORCEINLINE bool readVarUint( T& out ) {
        out = 0;
        uint8_t shift = 0;
        uint8_t b;
        do {
            if ( !this->read( b ) ) {
                this->invalidate( );
                return false;
            }
            out |= ( b & 0x7FLu ) << shift;
            shift += 7;
        } while ( ( b & 0x80 ) != 0 );
        return true;
    }
    FORCEINLINE bool readString( std::string_view& out ) {
        size_t size = 0;
        if ( !this->readVarUint( size ) )
            return false;
        void* ptr = nullptr;
        if ( !this->readBytesNoCopy( ptr, size ) )
            return false;
        out = { (const char*)ptr, size };
        return true;
    }

    FORCEINLINE custom_stream createNestedStream( size_t size ) {
        return { GetPointer( this->data, std::min( this->size( ), this->pos ) ), std::min( this->remaining( ), size ) };
    }

private:
    const void* data;
    size_t size_;
    size_t pos;
};