#pragma once

#include "CEconItem.hpp"
#include "C_BaseModelEntity.hpp"

class C_BaseCombatCharacter : public C_BaseModelEntity
{
public:
	CUtlVector<CHandle> wearables( ) {
		if ( !util::exists( this ) ) return CUtlVector<CHandle>{};

		return *( CUtlVector<CHandle>* )( (uintptr_t)this + schema::C_BaseCombatCharacter::m_hMyWearables );
	}

	int blood_color(){
		if ( !util::exists( this ) ) return -1;

		return Member<int>( schema::C_BaseCombatCharacter::m_bloodColor );
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
		return (C_EconItemView* )( (uintptr_t)this + schema::CAttributeContainer::m_Item );
	}
};

class C_EconEntity : VClass
{
public:
	CAttributeContainer* attribute_container( ) {
		return (CAttributeContainer*)( (uintptr_t)this + schema::C_EconEntity::m_AttributeManager );
	}
};