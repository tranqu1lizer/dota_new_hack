#pragma once

#include "NormalClass.hpp"
#include "../util/util.hpp"
#include "valve/CUtlStringToken.hpp"

class IMaterial2 {
public:
	virtual const char* GetName( ) = 0; // 0x0
	virtual const char* GetNameWithMod( ) = 0; // 0x8
	virtual void* GetMode( CUtlStringToken* ) = 0; // 0x10
	virtual void* GetAttributes( ) = 0; // 0x18
	virtual void vf4( ) = 0; // 0x20
	virtual void vf5( ) = 0; // 0x28
	virtual void vf6( ) = 0; // 0x30
	virtual void vf7( ) = 0; // 0x38
	virtual void vf8( ) = 0; // 0x40
	virtual void vf9( ) = 0; // 0x48
	virtual void vf10( ) = 0; // 0x50
	virtual void vf11( ) = 0; // 0x58
	virtual void vf12( ) = 0; // 0x60
	virtual void vf13( ) = 0; // 0x68
	virtual void vf14( ) = 0; // 0x70
	virtual void vf15( ) = 0; // 0x78
	virtual void vf16( ) = 0; // 0x80
	virtual void vf17( ) = 0; // 0x88
	virtual void* GetVertexShaderInputSignature( ) = 0; // 0x90
	virtual void vf19( ) = 0; // 0x98
	virtual void vf20( ) = 0; // 0xa0
	virtual void vf21( ) = 0; // 0xa8
	virtual void vf22( ) = 0; // 0xb0
	virtual void vf23( ) = 0; // 0xb8
	virtual void vf24( ) = 0; // 0xc0
	virtual void vf25( ) = 0; // 0xc8
	virtual void vf26( ) = 0; // 0xd0
	virtual void vf27( ) = 0; // 0xd8
	virtual void vf28( ) = 0; // 0xe0
	virtual void vf29( ) = 0; // 0xe8
	virtual void vf30( ) = 0; // 0xf0
	virtual void vf31( ) = 0; // 0xf8
	virtual void vf32( ) = 0; // 0x100
	virtual void vf33( ) = 0; // 0x10
};

class IMaterialSystem2 : VClass {
public:
	static auto get( )
	{
		static IMaterialSystem2* inst = static_cast<IMaterialSystem2*>( util::get_interface( "materialsystem2.dll", "VMaterialSystem2_001" ) );
		return inst;
	}

	IMaterial2*** FindOrCreateMaterialFromResource( IMaterial2*** material, const char* material_name ) {
		return CallVFunc<14, IMaterial2***>( material, material_name );
	}
};