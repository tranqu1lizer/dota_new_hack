#pragma once

#include "C_BaseEntity.hpp"

class C_BaseCombatCharacter : public C_BaseEntity
{
	int GetBloodColor(){
		if ( !util::does_exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field( "client.dll/C_BaseCombatCharacter/m_bloodColor" );
		return Member<int>( offset );
	}
};