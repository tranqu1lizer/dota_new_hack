#pragma once

class CDOTA_Buff : public VClass {
public:
	const char* GetBuffName( ) noexcept {
		if ( !util::exists( this ) ) return nullptr;
		static const auto offset = schema::dynamic_field_offset( "server.dll/CDOTA_Buff/m_name" );
		return Member<const char*>( offset );
	}

	float GetDuration( ) noexcept {
		if ( !util::exists( this ) ) return -1;
		static const auto offset = schema::dynamic_field_offset( "server.dll/CDOTA_Buff/m_flDuration" );
		return Member<float>( offset );
	}

	CHandle GetCaster( ) {
		if ( !util::exists( this ) ) return CHandle{};
		static const auto offset = schema::dynamic_field_offset( "server.dll/CDOTA_Buff/m_hCaster" );
		return Member<CHandle>( offset );
	}
	
	CHandle GetAbility( ) {
		if ( !util::exists( this ) ) return CHandle{};
		static const auto offset = schema::dynamic_field_offset( "server.dll/CDOTA_Buff/m_hAbility" );
		return Member<CHandle>( offset );
	}

	CHandle GetParent( ) {
		if ( !util::exists( this ) ) return CHandle{};
		static const auto offset = schema::dynamic_field_offset( "server.dll/CDOTA_Buff/m_hParent" );
		return Member<CHandle>( offset );
	};
};

class CDOTA_ModifierManager : VClass {
public:
	CUtlVector<CDOTA_Buff*> GetModifiers( ) {
		if ( !util::exists( this ) ) return CUtlVector<CDOTA_Buff*>{};
		return Member<CUtlVector<CDOTA_Buff*>>( 0x10 );
	}

	bool HasModifier( const char* modifier_name ) {
		if ( !util::exists( this ) ) return false;

		for ( auto modifier : GetModifiers( ) ) {
			if ( const auto name = modifier->GetBuffName( ); name ) {
				if ( !strcmp( name, modifier_name ) ) return true;
			}
		}
		return false;
	}
};