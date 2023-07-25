#pragma once

#include "C_BaseCombatCharacter.hpp"
#include "CNetworkMessages.hpp"
#include "NormalClass.hpp"

class C_DOTA_BaseNPC : public C_BaseCombatCharacter
{
public:
	float max_mana( ) {
		if ( !util::exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field_offset( "client.dll/C_DOTA_BaseNPC/m_flMaxMana" );
		return Member<float>( offset );
	}

	float mana( ) {
		if ( !util::exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field_offset( "client.dll/C_DOTA_BaseNPC/m_flMana" );
		return Member<float>( offset );
	}

	bool moving( ) {
		if ( !util::exists( this ) ) return -1;
		
		return schema_member<bool>( "client.dll/C_DOTA_BaseNPC/m_bIsMoving" );
	}

	int movespeed( ) {
		if ( !util::exists( this ) ) return -1;

		return schema_member<int>( "client.dll/C_BaseEntity/m_iMoveSpeed" );
	}

	uint64_t GetUnitState( ) {
		if ( !util::exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field_offset( "client.dll/C_DOTA_BaseNPC/m_nUnitState64" );
		return Member<uint64_t>( offset );
	}

	int32_t GetUnitType( ) {
		if ( !util::exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field_offset( "client.dll/C_DOTA_BaseNPC/m_iUnitType" );
		return Member<int32_t>( offset );
	}

	CDOTA_ModifierManager* modifier_manager( ) {
		if ( !util::exists( this ) ) return nullptr;
		static const auto offset = schema::dynamic_field_offset( "client.dll/C_DOTA_BaseNPC/m_ModifierManager" );
		return MemberNotPtr<CDOTA_ModifierManager>( offset );
	}

	int health_bar_offset( ) noexcept {
		if ( !util::exists( this ) ) return -1;

		return schema_member<int>( "client.dll/C_DOTA_BaseNPC/m_iHealthBarOffset" );
	}

	float physical_armor( ) {
		if ( !util::exists( this ) ) return -1;
		return CallVFunc<271, float>( );
	}

	float magical_armor( ) {
		if ( !util::exists( this ) ) return -1;
		return CallVFunc<272, float>( );
	}

	CUtlVector<CHandle> old_wearables( ) {
		if ( !util::exists( this ) ) return CUtlVector<CHandle>{};

		static const auto offset = schema::dynamic_field_offset( "client.dll/C_DOTA_BaseNPC/m_hOldWearables" );
		return *( CUtlVector<CHandle>* )( (uintptr_t)this + offset );
	}

	EntityIndex_t ability_at( const int idx )
	{
		if ( !util::exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field_offset( "client.dll/C_DOTA_BaseNPC/m_hAbilities" );
		const auto& ability = Member<CHandle>( offset + ( 4 * idx ) );
		return ability.ToIndex( );
	}

	EntityIndex_t item_in_slot( const int idx )
	{
		if ( !util::exists( this ) ) return -1;
		if ( Member<bool>( schema::dynamic_field_offset( "client.dll/C_DOTA_BaseNPC/m_bHasInventory" ) ) ) {
			static const auto offset = schema::dynamic_field_offset( "client.dll/C_DOTA_BaseNPC/m_Inventory" );

			const auto item = Member<CHandle>( offset + ( 4 * idx ) );
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

	bool illusion( ) {
		if ( !util::exists( this ) ) return false;
		static const auto offset = schema::dynamic_field_offset( "client.dll/C_DOTA_BaseNPC_Hero/m_hReplicatingOtherHeroModel" );
		const auto state = Member<uintptr_t>( offset ) != 0xFFFFFFFF;
		return state;
	}

	int32_t hero_id( ) {
		if ( !util::exists( this ) ) return -1;

		return schema_member<int32_t>( "client.dll/C_DOTA_BaseNPC_Hero/m_iHeroID" );
	}
};