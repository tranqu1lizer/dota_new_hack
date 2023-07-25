#pragma once

#include "NormalClass.hpp"

typedef std::uint64_t ResourceId_t;

template<std::size_t Size>
class ResourceType_t
{
public:
	std::array<char, 8> value{};

	constexpr ResourceType_t( ) = default;
	constexpr ResourceType_t( const char( &Str )[ Size ] )
	{
		std::ranges::copy( Str, value.begin( ) );
	}
public:
	constexpr const auto& get( ) const noexcept { return *reinterpret_cast<const std::uint64_t*>( value.data( ) ); }

	bool operator==( std::uint64_t other ) {
		return get( ) == other;
	}
};

class IResourceSystem : VClass {
	PDEFINE_INTERFACE( IResourceSystem, "resourcesystem.dll", "ResourceSystem013" );

	template<ResourceType_t resource_type>
	auto GetResourceByHash( const ResourceId_t id ) const
	{
		const auto ret = CallVFunc<32, uintptr_t>( id, resource_type.get( ) );
		const auto str_name = reinterpret_cast<const char*>( *reinterpret_cast<uintptr_t*>( *reinterpret_cast<uintptr_t*>( ret + 0x8 ) ) );
		return str_name;
	}

	const char* GetResourceName( std::uint64_t resource ) {
		if ( !util::exists( (void*)resource ) || !util::exists( (void*)( resource + 0x8 ) ) )
			return "error";

		return *reinterpret_cast<const char**>( *reinterpret_cast<uintptr_t*>( resource + 0x8 ) );
	}

	uint64 GetResourceType( std::uint64_t resource ) {
		return CallVFunc<42, uint64>( resource );
	}

	const uint32_t GetNamedResourceCount( ) {
		return CallVFunc<33, uint32_t>( );
	}

	auto GetNamedResources( ) {
		std::array<std::uint64_t, 2048> resources{};

		CallVFunc<34, uint32_t>( 0, resources.size( ), resources.data( ), 1 );
		return resources;
	}
};