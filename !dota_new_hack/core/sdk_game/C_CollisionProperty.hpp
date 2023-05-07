#pragma once

#include "ISchemaSystem.hpp"

class C_CollisionProperty
{
public:
    virtual CSchemaClassBinding* Schema_DynamicBinding( void ) = 0;
    virtual void* GetDataMapOffset( ) = 0;

    inline const vector3d* GetMins( )
    {
        if ( !this ) return nullptr;

        return (const vector3d*)((uintptr_t)this + 0x40);
    }

    inline const vector3d* GetMaxs( )
    {
        if ( !this ) return nullptr;

        return (const vector3d*)((uintptr_t)this + 0x4C);
    }
};