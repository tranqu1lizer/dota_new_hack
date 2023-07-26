#pragma once

#include <codecvt>
#include "../ICVar.hpp"

class CPanel2D : public VClass
{
public:
	OFFSET( CUIPanel*, UIPanel, 0x8 );

	template<class T> void SetDialogVariable( const std::string_view& variable, T value ) {
		UIPanel( )->SetDialogVariable( variable.data( ), value );
	}

	bool DeleteAsync( float delay = 0.f ) {
		if ( !global::patterns::CPanel2D__DeleteAsync ) return false;
		typedef bool( __fastcall* fn_t )( CPanel2D*, float );
		return reinterpret_cast<fn_t>( global::patterns::CPanel2D__DeleteAsync )( this, delay );
	}
};

class CLabel : public CPanel2D {
public:
	void SetLabelText( const char* text ) {
		CallVFunc<77>( text );
	}
	const char* GetLabelText( ) {
		return CallVFunc<79, const char*>( );
	}
};

class CButton : public CPanel2D {
public:

};

class CSlider : public CPanel2D {
public:
	void SetFloat( float vl ) {
		if ( !global::patterns::CSlider__SetValue ) return;

		reinterpret_cast<void( __fastcall* )( CSlider*, float )>( global::patterns::CSlider__SetValue )( this, vl );
	}

	float GetFloat( ) {
		return Member<float>( 0x4c );
	}
};

class CTextEntry : public CPanel2D {
public:
	void SetText( const char* text ) {
		if ( !global::patterns::CTextEntry__SetText ) return;
		reinterpret_cast<void( __fastcall* )( CTextEntry*, const char* )>( global::patterns::CTextEntry__SetText )( this, text );
	}

	std::string GetDisplayText( ) {
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
		const char32_t const* ptr = reinterpret_cast<const char32_t* ( __fastcall* )( CTextEntry* )>( global::patterns::CTextEntry__GetText )( this );

		return conv.to_bytes( ptr );
	}
};

class CImagePanel : public CPanel2D {
public:
	std::uintptr_t SetSource( const char* src ) {
		if ( !global::patterns::CImagePanel__SetImage ) return 0;
		return reinterpret_cast<std::uintptr_t( __fastcall* )( CImagePanel*, const char* )>( global::patterns::CImagePanel__SetImage )( this, src );
	}

	const char* GetSourceText( ) {
		return Member<const char*>( 0x98 );
	}
};

class CDOTA_UI_AbilityImage : public CImagePanel {
public:
	void SetName( const char* ability_name ) {
		if ( !global::patterns::CDOTA_UI_AbilityImage__SetAbilityName ) return;
		reinterpret_cast<void( __fastcall* )( CDOTA_UI_AbilityImage*, const char* )>( global::patterns::CDOTA_UI_AbilityImage__SetAbilityName )( this, ability_name );
	}
};

class CDOTA_UI_ItemImage : public CImagePanel {
public:
	unsigned int* SetName( const char* item_name ) {
		if ( !global::patterns::CDOTA_UI_ItemImage__SetItemByName ) return nullptr;
		return reinterpret_cast<unsigned int* ( __fastcall* )( CDOTA_UI_ItemImage*, const char* )>( global::patterns::CDOTA_UI_ItemImage__SetItemByName )( this, item_name );
	}
};

class C_DOTAGameManager;

class CDOTA_UI_HeroImage : public CImagePanel {
public:
	C_DOTAGameManager* SetName( const char* hero_name ) {
		if ( !global::patterns::CDOTA_UI_HeroImage__SetHeroName ) return nullptr;
		return reinterpret_cast<C_DOTAGameManager * ( __fastcall* )( CDOTA_UI_HeroImage*, const char* )>( global::patterns::CDOTA_UI_HeroImage__SetHeroName )( this, hero_name );
	}
};

class CDOTA_DB_SettingsCheckbox : public CButton {
public:
	void SetSelected( bool state ) {
		CallVFunc<66>( state );
	}
};

class CDropDown : public CPanel2D {
public:
	inline static CUIPanel* ( *aCDropDown__GetSelected )( CDropDown* rcx ) = nullptr;

	CUIPanel* GetSelected( ) {
		return aCDropDown__GetSelected( this );
	}
};

class CDOTA_Hud_Top_Bar : public CPanel2D {
public:
	CPanel2D* GetGoldPanel( ) {
		return Member<CPanel2D*>( 0x110 );
	}

	void UpdateNetWorthDifference( int RadiantNetWorth, int DireNetWorth ) {

		if ( !GetGoldPanel( ) )
			return;

		GetGoldPanel( )->UIPanel( )->SwitchClass( "gold_delta_mode", "GoldDeltaDiscrete" );
		GetGoldPanel( )->UIPanel( )->SwitchClass( "leading_team", RadiantNetWorth <= DireNetWorth ? "DireLeadingGold" : "RadiantLeadingGold" );
		GetGoldPanel( )->UIPanel( )->SetActive( ( RadiantNetWorth + DireNetWorth ) );

		const int absDiffirence = std::abs( RadiantNetWorth - DireNetWorth );

		if ( absDiffirence >= 1000 )
		{
			char Buffer[ 16 ];
			sprintf_s( Buffer, "%dk", ( absDiffirence / 1000 ) );
			GetGoldPanel( )->SetDialogVariable( "gold_delta_clamped_string", Buffer );
		}
		else
		{
			GetGoldPanel( )->SetDialogVariable( "gold_delta_clamped_string", "<1k" );
		}
	}
};

class CDOTA_Hud_ErrorMsg : public CPanel2D {
	char pad0008[ 0x28 ]; // 0x8
public:
	float m_flTotalTime; // 0x30
	float m_flErrorDurationTime; // 0x34

	void ShowErrorMessage( char* const content ) {
		UIPanel( )->RemoveClass( "PopOutEffect" );
		UIPanel( )->ApplyStyles( 1 );
		UIPanel( )->AddClass( "ShowErrorMsg" );

		UIPanel( )->RemoveClass( "Hidden" );
		SetDialogVariable( "error_msg", content );
		UIPanel( )->AddClass( "PopOutEffect" );

		const float flTime = util::Plat_FloatTime( );
		m_flTotalTime = flTime + m_flErrorDurationTime;
	}
};