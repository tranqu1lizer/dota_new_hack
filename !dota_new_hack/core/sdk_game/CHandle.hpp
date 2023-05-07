#pragma once

#include <cstdint>
#include <limits>

#include "EntityIndex.hpp"

constexpr auto MAX_ENTITIES_PER_LIST = 512;
constexpr auto MAX_ENTITY_LISTS = 64;
constexpr auto ENTITYSYSTEM_CAPACITY = MAX_ENTITY_LISTS * MAX_ENTITIES_PER_LIST;

class CHandle
{
    std::uint32_t data{};
    static inline constexpr auto ENTITYHANDLE_MASK = ENTITYSYSTEM_CAPACITY - 1; //0x7FFF
public:
    EntityIndex_t GetIndex() const noexcept
    {
        return { data & ENTITYHANDLE_MASK };
    }
    bool IsValid() const noexcept
    {
        return data != std::numeric_limits<std::uint32_t>::max();
    }
};