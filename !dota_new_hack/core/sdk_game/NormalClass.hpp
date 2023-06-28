#pragma once

#include <string>
#include "SomeFunction.h"

class NormalClass
{
	virtual void dummy_fn( ) = 0; // so that the classes have a vtable
public:
	template<typename T>
	T& Field( int offset ) {
		return *(T*)( (uintptr_t)this + offset );
	}

	template<class T>
	auto& Member( std::ptrdiff_t offset ) noexcept
	{
		return *(T*)( (std::uintptr_t)this + offset ); 
	}

	template<class T>
	const auto& Member( std::ptrdiff_t offset ) const noexcept
	{
		return *reinterpret_cast<const T*>( reinterpret_cast<std::uintptr_t>( this ) + offset );
	}

	template<typename T>
	T* MemberNotPtr( std::ptrdiff_t offset ) {
		return reinterpret_cast<T*>( reinterpret_cast<std::uintptr_t>( this ) + offset );
	}
	
	template<typename T>
	auto MemberInline( std::ptrdiff_t offset ) {
		return ( T* )( (uintptr_t)this + offset );
	}
};

class VClass : public NormalClass
{
public:
	some_function GetVFunc( int index )
	{
		const uintptr_t vtable = *( (uintptr_t*)( this ) );
		uintptr_t entry = vtable + sizeof( uintptr_t ) * index;
		return some_function( *(uintptr_t*)entry );
	}

	template<uint32_t index, typename RET = uintptr_t, typename ...T>
	RET CallVFunc( T... t ) {
		return GetVFunc( index ).call<RET>( this, t... );
	}

	template <typename T = std::uintptr_t>
	inline T GetVF( int Index )
	{
		return (T)util::vmt( (std::uintptr_t)this, Index );
	}
};

class SchemaVClass : public VClass { //
public:
	template<class T>
	auto& schema_member( const std::string& path ) noexcept
	{
		const auto offset = schema::dynamic_field_offset( path );
		return *(T*)( (std::uintptr_t)this + offset );
	}
};

#define GETTER(type, name, offset) type name() {\
if ( util::exists( reinterpret_cast<std::uintptr_t*>( this ) ) )\
    return Member<type>( offset );\
throw std::runtime_error{ "getter err" }; }

#define FIELD(type, name, offset) type& name() { return Field<type>(offset); }