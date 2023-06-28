#pragma once

#include "C_BaseEntity.hpp"
#include "CEconItem.hpp"

class C_BaseCombatCharacter : public C_BaseEntity
{
public:
	CUtlVector<CHandle> wearables( ) {
		if ( !util::exists( this ) ) return CUtlVector<CHandle>{};

		static const auto offset = schema::dynamic_field_offset( "client.dll/C_BaseCombatCharacter/m_hMyWearables" );
		return *( CUtlVector<CHandle>* )( (uintptr_t)this + offset );
	}

	int blood_color(){
		if ( !util::exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field_offset( "client.dll/C_BaseCombatCharacter/m_bloodColor" );
		return Member<int>( offset );
	}

	inline void wearables_changed( ) {
		if ( !util::exists( this ) ) return;
		CallVFunc<249>( );
	}
};

class CAttributeContainer : VClass
{
public:
	C_EconItemView* econ_item( ) {
		static const auto offset = schema::dynamic_field_offset( "client.dll/CAttributeContainer/m_Item" );
		return (C_EconItemView* )( (uintptr_t)this + offset );
	}
};

class C_EconEntity : VClass
{
public:
	CAttributeContainer* attribute_container( ) {
		static const auto offset = schema::dynamic_field_offset( "client.dll/C_EconEntity/m_AttributeManager" );
		return (CAttributeContainer*)( (uintptr_t)this + offset );
	}
};