#pragma once

#include "C_BaseEntity.hpp"

class C_DOTABaseAbility : public C_BaseEntity
{
public:
	const std::string& GetIcon( ) {
		std::string icon_name = "file://{images}/spellicons/";
		icon_name.append( this->identity( )->GetName( ) );
		return icon_name.append( ".png" );
	}

	int GetLevel( ) {
		if ( !util::does_exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field( "client.dll/C_DOTABaseAbility/m_iLevel" );
		return Member<int>( offset );
	}

	int GetManaCost( ) {
		if ( !util::does_exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field( "client.dll/C_DOTABaseAbility/m_iManaCost" );
		return Member<int>( offset );
	}

	int GetCastRange( ) {
		if ( !util::does_exists( this ) || !functions_call::GetLevelSpecialValueFor ) return -1;
		const auto range = functions_call::GetLevelSpecialValueFor( this, "AbilityCastRange", this->GetLevel( ) - 1, nullptr, nullptr, nullptr );
		return static_cast<int>( range );
	}

	float GetCooldown( ) {
		if ( !util::does_exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field( "client.dll/C_DOTABaseAbility/m_fCooldown" );
		return Member<float>( offset );
	}

	float GetCooldownLength( ) {
		if ( !util::does_exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field( "client.dll/C_DOTABaseAbility/m_flCooldownLength" );
		return Member<float>( offset );
	}

	bool IsInAbilityPhase( ) {
		if ( !util::does_exists( this ) ) return false;
		static const auto offset = schema::dynamic_field( "client.dll/C_DOTABaseAbility/m_bInAbilityPhase" );
		return Member<bool>( offset );
	}

	bool IsHidden( ) {
		if ( !util::does_exists( this ) ) return false;
		static const auto offset = schema::dynamic_field( "client.dll/C_DOTABaseAbility/m_bHidden" );
		return Member<bool>( offset );
	}

	bool IsActivated( ) {
		if ( !util::does_exists( this ) ) return false;
		static const auto offset = schema::dynamic_field( "client.dll/C_DOTABaseAbility/m_bActivated" );
		return Member<bool>( offset );
	}
};

class C_DOTA_Item : public C_DOTABaseAbility {
public:

	bool IsValidItem( ) {
		if ( !util::does_exists( this ) ) return false;
		return util::fast_strstr( this->schema_dynamic_binding( )->m_binary_name, "C_DOTA_Item" );
	}

	bool IsItemEnabled( ) {
		if ( !util::does_exists( this ) ) return false;
		static const auto offset = schema::dynamic_field( "client.dll/C_DOTA_Item/m_bItemEnabled" );
		return Member<bool>( offset );
	}
};