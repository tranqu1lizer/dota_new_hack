#pragma once

#include "ISchemaSystem.hpp"

class C_BaseEntity;

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

class CParticleMgr;
class C_BaseEntityOuterHelper {
    void* pad;
public:
    C_BaseEntity* m_pEntity; // 0x8
};

struct C_ParticleProperty {
public:
    CParticleMgr* m_pParticleMgr; // 0x0
    C_BaseEntityOuterHelper* m_pOuter; // 0x8
private:
    int64_t some_boolean; // 0x10
    void* unk1; // 0x18
    void* unk2; // 0x20
    void* unk3; // 0x28

}; // size: 0x28