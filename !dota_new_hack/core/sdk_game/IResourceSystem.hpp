#pragma once

#include "NormalClass.hpp"

typedef uint64_t ResourceId_t;

template<std::size_t Size>
class ResourceType_t
{
public:
	std::array<char, 8> value{};

	constexpr ResourceType_t( ) = default;
	constexpr ResourceType_t( const char( &Str )[Size] )
	{
		std::ranges::copy( Str, value.begin( ) );
	}
public:
	constexpr const auto& Get( ) const noexcept { return *reinterpret_cast<const std::uint64_t*>(value.data( )); }
};

class IResourceSystem : VClass {
	static auto GetInstanceImpl( )
	{
		static IResourceSystem* inst = nullptr;
		if ( !inst ) inst = static_cast<IResourceSystem*>( util::get_interface( "resourcesystem.dll", "ResourceSystem013" ) );

		return inst;
	}
public:
	static auto& GetInstance( )
	{
		return *GetInstanceImpl( );
	}

	template<ResourceType_t resource_type>
	auto GetResourceByHash( const ResourceId_t id ) const
	{
		const auto ret = CallVFunc<32, uintptr_t>( id, resource_type.Get( ) );
		const auto str_name = reinterpret_cast<const char*>(*reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(ret + 0x8)));
		return str_name;
	}
};