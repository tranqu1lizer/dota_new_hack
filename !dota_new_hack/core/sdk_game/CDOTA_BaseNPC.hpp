#pragma once

#include "C_BaseCombatCharacter.hpp"
#include "CNetworkMessages.hpp"
#include "NormalClass.hpp"

class C_DOTA_BaseNPC : public C_BaseCombatCharacter
{
public:
	OFFSET( float, GetMaxMana, schema::C_DOTA_BaseNPC::m_flMaxMana );
	OFFSET( float, GetMana, schema::C_DOTA_BaseNPC::m_flMana );
	OFFSET( bool, IsMoving, schema::C_DOTA_BaseNPC::m_bIsMoving );
	OFFSET( int, GetMoveSpeed, schema::C_DOTA_BaseNPC::m_iMoveSpeed );
	OFFSET( uint64_t, GetUnitState, schema::C_DOTA_BaseNPC::m_nUnitState64 );
	OFFSET( int32_t, GetUnitType, schema::C_DOTA_BaseNPC::m_iUnitType );
	OFFSET_INLINE( CDOTA_ModifierManager, GetModifierManager, schema::C_DOTA_BaseNPC::m_ModifierManager );
	OFFSET( int, GetHBOffset, schema::C_DOTA_BaseNPC::m_iHealthBarOffset );

	float GetPhysicalArmor( ) {
		if ( !util::exists( this ) ) return -1;
		return CallVFunc<271, float>( );
	}

	float GetMagicalArmor( ) {
		if ( !util::exists( this ) ) return -1;
		return CallVFunc<272, float>( );
	}

	EntityIndex_t GetAbility( const int idx )
	{
		if ( !util::exists( this ) ) return -1;
		const auto& ability = Member<CHandle>( schema::C_DOTA_BaseNPC::m_hAbilities + ( 4 * idx ) );
		return ability.ToIndex( );
	}

	EntityIndex_t GetItemSlot( const int idx )
	{
		if ( !util::exists( this ) ) return -1;
		if ( Member<bool>( schema::C_DOTA_BaseNPC::m_bHasInventory ) ) {

			const auto& item = Member<CHandle>( schema::C_DOTA_BaseNPC::m_Inventory + ( 4 * idx ) );
			if ( item.is_valid( ) ) return item.ToIndex( );
		}
		return -1;
	}

};

class C_DOTA_BaseNPC_Hero : public C_DOTA_BaseNPC {
public:
	int ClientThink( ) {
		return CallVFunc<127, int>( );
	}

	bool IsIllusion( ) {
		if ( !util::exists( this ) ) return false;
		const auto state = Member<std::uintptr_t>( schema::C_DOTA_BaseNPC_Hero::m_hReplicatingOtherHeroModel ) != 0xFFFFFFFF;
		return state;
	}

	OFFSET( int32_t, GetHeroId, schema::C_DOTA_BaseNPC_Hero::m_iHeroID )
};