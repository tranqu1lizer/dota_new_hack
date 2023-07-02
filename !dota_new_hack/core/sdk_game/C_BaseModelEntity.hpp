#pragma once

#include "C_BaseEntity.hpp"
#include "NormalClass.hpp"
#include "CNetworkMessages.hpp"
#include "C_Properties.hpp"
#include "../util/util.hpp"

class CModelState
{
private:
	[[maybe_unused]] uint8_t __pad0000[ 0xa0 ]; // 0x0
public:
	void* m_hModel; // 0xa0	
	CUtlSymbolLarge m_ModelName; // 0xa8	
private:
	[[maybe_unused]] uint8_t __pad00b0[ 0x38 ]; // 0xb0
public:
	bool m_bClientClothCreationSuppressed; // 0xe8	
private:
	[[maybe_unused]] uint8_t __pad00e9[ 0x97 ]; // 0xe9
public:
	uint64_t m_MeshGroupMask; // 0x180	
private:
	[[maybe_unused]] uint8_t __pad0188[ 0x9a ]; // 0x188
public:
	int8_t m_nIdealMotionType; // 0x222	
	int8_t m_nForceLOD; // 0x223	
	int8_t m_nClothUpdateFlags; // 0x224	
};

class C_BaseModelEntity : public C_BaseEntity {
public:
	C_CollisionProperty* collision( ) {
		return Member< C_CollisionProperty* >( 0x310 );
	}
	void set_color( const unsigned short r, const unsigned short g, const unsigned short b, const unsigned short a = 255 ) {
		if ( !util::exists( this ) ) return;
		static const auto offset = schema::dynamic_field_offset( "client.dll/C_BaseModelEntity/m_clrRender" );
		const auto base = ( (std::uintptr_t)this + offset );
		*(std::uint8_t*)( base + 0 ) = static_cast<BYTE>( r );
		*(std::uint8_t*)( base + 1 ) = static_cast<BYTE>( g );
		*(std::uint8_t*)( base + 2 ) = static_cast<BYTE>( b );
		*(std::uint8_t*)( base + 3 ) = static_cast<BYTE>( a );

		CNetworkMessages::get( )->find_network_callback( "OnColorChanged" )( this );
	}

	void set_mesh_group_mask( uint64 msk ) {
		static some_function set_mesh_group_mask = util::get_absolute_address( util::find_pattern( "client.dll", "E8 ? ? ? ? 48 8B 07 48 8D 55 7F" ), 1, 5 );
		if ( !set_mesh_group_mask.valid( ) ) return;
		set_mesh_group_mask( this->schema_member<SchemaVClass*>( "client.dll/C_BaseEntity/m_pGameSceneNode" ), msk );
	}

	auto model_state( ) {
		return this->schema_member<SchemaVClass*>( "client.dll/C_BaseEntity/m_pGameSceneNode" )->MemberInline<CModelState>( schema::dynamic_field_offset( "client.dll/CSkeletonInstance/m_modelState" ) );
	}
};