#include <windows.h>
#include <optional>
#include <string_view>
#include "util.hpp"

class AddressWrapper {
public:
	std::uintptr_t ptr;
	AddressWrapper( std::uintptr_t ptr ) : ptr( ptr ) {}
	AddressWrapper( void* ptr ) : ptr( reinterpret_cast<std::uintptr_t>( ptr ) ) {}
	template<typename T>
	operator T( ) const
	{
		return (T)ptr;
	}
	AddressWrapper get_offset( std::ptrdiff_t offset ) const {
		return AddressWrapper( ptr + offset );
	}

	template<typename T = AddressWrapper>
	T get_address_from_instruction_ptr( std::ptrdiff_t addrOffset, std::optional<uint32_t> opcodeSize = std::nullopt ) const {
		return T( ptr + *(int*)( ptr + addrOffset ) + opcodeSize.value_or( addrOffset + sizeof( std::uint32_t ) ) );
	}

	template<typename T>
	void set( const T& val ) {
		*(T*)ptr = val;
	}

	template<typename T>
	T* get_as( ) const {
		return (T*)ptr;
	}
};