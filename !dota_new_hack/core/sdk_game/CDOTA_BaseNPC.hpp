#pragma once

#include "C_BaseCombatCharacter.hpp"

class C_DOTA_BaseNPC : public C_BaseCombatCharacter
{
public:
	float max_mana( ) {
		if ( !util::does_exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field( "client.dll/C_DOTA_BaseNPC/m_flMaxMana" );
		return Member<float>(offset);
	}

	float mana( ) {
		if ( !util::does_exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field( "client.dll/C_DOTA_BaseNPC/m_flMana" );
		return Member<float>( offset );
	}

	uint64_t GetUnitState( ) {
		if ( !util::does_exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field( "client.dll/C_DOTA_BaseNPC/m_nUnitState64" );
		return Member<uint64_t>( offset );
	}

	int32_t GetUnitType( ) {
		if ( !util::does_exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field( "client.dll/C_DOTA_BaseNPC/m_iUnitType" );
		return Member<int32_t>( offset );
	}

	CDOTA_ModifierManager* GetModifierManager( ) {
		if ( !util::does_exists( this ) ) return nullptr;
		static const auto offset = schema::dynamic_field( "client.dll/C_DOTA_BaseNPC/m_ModifierManager" );
		return MemberNotPtr<CDOTA_ModifierManager>( offset );
	}

	int health_bar_offset( ) noexcept {
		if ( !util::does_exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field( "client.dll/C_DOTA_BaseNPC/m_iHealthBarOffset" );
		return Member<int>( offset );
	}

	void SetEntityColor( const unsigned short r, const unsigned short g, const unsigned short b, const unsigned short a = 255 ) {
		if ( !util::does_exists( this ) ) return;
		static const auto offset = schema::dynamic_field( "client.dll/C_BaseModelEntity/m_clrRender" );

		Member<int>( offset ) = r | ( g << 8 ) | ( b << 16 ) | ( a << 24 );
		functions_helper::OnColorChanged( this );
	}

	void SetIllusionManaBarVision( bool state ) {
		if ( !util::does_exists( this ) ) return;
		static const auto offset = schema::dynamic_field( "client.dll/C_DOTA_BaseNPC/m_bHasClientSeenIllusionModifier" );

		Member<bool>( offset ) = state;
	}

	bool IsHero( ) {
		if ( !util::does_exists( this ) ) return false;
		return this->GetUnitType( ) == 1;
	}

	bool IsLaneCreep( ) {
		if ( !util::does_exists( this ) ) return false;
		return this->GetUnitType( ) == 1152;
	}

	float GetPhysicalArmorValue( ) {
		if ( !util::does_exists( this ) ) return -1;
		return CallVFunc<271, float>( );
	}

	float GetMagicalArmorValue( ) {
		if ( !util::does_exists( this ) ) return -1;
		return CallVFunc<272, float>( );
	}

	EntityIndex_t GetAbility( const int idx )
	{
		if ( !util::does_exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field( "client.dll/C_DOTA_BaseNPC/m_hAbilities" );
		const auto& ability = Member<CHandle>( offset + ( 4 * idx ) );
		return ability.GetIndex( );
	}

	EntityIndex_t GetItem( const int idx )
	{
		if ( !util::does_exists( this ) ) return -1;
		if ( Member<bool>( schema::dynamic_field( "client.dll/C_DOTA_BaseNPC/bHasInventory" ) ) ) {
			static const auto offset = schema::dynamic_field( "client.dll/C_DOTA_BaseNPC/m_Inventory" );

			const auto item = Member<CHandle>( offset + ( 4 * idx ) );
			if ( item.IsValid( ) ) return item.GetIndex( );
		}
		return -1;
	}

};

class C_DOTA_BaseNPC_Hero : public C_DOTA_BaseNPC {
public:
	bool IsIllusion( ) {
		if ( !util::does_exists( this ) ) return false;
		static const auto offset = schema::dynamic_field( "client.dll/C_DOTA_BaseNPC_Hero/m_hReplicatingOtherHeroModel" );
		return Member<uintptr_t>( offset ) != 0xFFFFFFFF;
	}
};