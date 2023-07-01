#include <cstdint>

class some_function {
	void* ptr;
public:
	some_function( std::uintptr_t ptr ) : ptr( (void*)ptr ) { }
	some_function( void* ptr ) : ptr( ptr ) { }

	operator void* ( ) {
		return ptr;
	}

	bool valid( ) {
		return ptr != nullptr;
	}

	template<typename ...T>
	void* __fastcall operator()( T... t ) {
		if ( !ptr ) return nullptr;
		return (void*)( ( uintptr_t( __fastcall* )( T... ) )ptr )( t... );
	}
	// Used to specify the return type(e. g. in case of a floating-point value)
	template<typename V, typename ...T>
	V __fastcall call( T... t ) {
		if ( ptr ) {
			return ( ( V( __fastcall* )( T... ) )ptr )( t... );
		}
	}

};