#pragma once

#include <cstdint>
#include <limits>

#include "EntityIndex.hpp"

constexpr auto MAX_ENTITIES_PER_LIST = 512;
constexpr auto MAX_ENTITY_LISTS = 64;
constexpr auto ENTITYSYSTEM_CAPACITY = MAX_ENTITY_LISTS * MAX_ENTITIES_PER_LIST;

#undef max

class CHandle
{
    std::uint32_t _data{};
    static inline constexpr auto ENTITYHANDLE_MASK = ENTITYSYSTEM_CAPACITY - 1; //0x7FFF
public:
    CHandle( std::uint32_t data = -1 ) : _data( data ) {}

    EntityIndex_t to_index() const noexcept {
        return { _data & ENTITYHANDLE_MASK };
    }
    bool is_valid() const noexcept {
        return _data != std::numeric_limits<std::uint32_t>::max();
    }
    std::uint32_t data( ) { return _data; }
};