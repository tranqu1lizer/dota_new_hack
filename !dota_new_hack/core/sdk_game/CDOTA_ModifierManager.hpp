#pragma once

#include "NormalClass.hpp"
#include "valve/CUtlVector.hpp"
#include "ISchemaSystem.hpp"
#include "CHandle.hpp"
#include "../schema.h"

class CDOTA_Buff {
public:
	OFFSET( const char*, GetBuffName, schema::CDOTA_Buff::m_name );
	OFFSET( float, GetDuration, schema::CDOTA_Buff::m_flDuration );
	OFFSET( CHandle, GetCaster, schema::CDOTA_Buff::m_hCaster );
	OFFSET( CHandle, GetAbility, schema::CDOTA_Buff::m_hAbility );
	OFFSET( CHandle, GetParent, schema::CDOTA_Buff::m_hParent );
};

class CDOTA_ModifierManager : VClass {
public:
	OFFSET( CUtlVector<CDOTA_Buff*>, GetModifiers, 0x10 );

	bool HasModifier( const char* modifier_name ) {
		if ( !util::exists( this ) ) return false;

		for ( auto modifier : GetModifiers( ) ) {
			if ( const auto name = modifier->GetBuffName( ); name ) {
				if ( !util::fast_strcmp( name, modifier_name ) ) return true;
			}
		}
		return false;
	}
};