#pragma once

#include <cstdint>

class EntityIndex_t
{
	std::int32_t value{};
public:
	EntityIndex_t( ) = default;
	EntityIndex_t( std::int32_t v ) noexcept : value{ v } {}
	auto Get( ) const noexcept
	{
		return value;
	}
	bool is_valid( ) const noexcept
	{
		return value >= 0;
	}

	auto& operator++( ) noexcept
	{
		value++;
		return *this;
	}
	auto operator++( int ) noexcept
	{
		EntityIndex_t copy{ *this };
		operator++( );
		return copy;
	}

	constexpr friend auto operator<=>( const EntityIndex_t&, const EntityIndex_t& ) = default;
};