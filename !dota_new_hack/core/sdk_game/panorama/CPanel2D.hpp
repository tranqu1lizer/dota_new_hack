#pragma once

#include <codecvt>

class CPanel2D : public VClass
{
public:
	GETTER( CUIPanel*, ui_panel, 0x8 );

	inline static void( *aSetDialogVariableInt )( CPanel2D*, const char*, unsigned int ) = nullptr;

	void SetDialogVariable( const char* variable, unsigned int value ) {
		if ( !aSetDialogVariableInt ) return;
		aSetDialogVariableInt( this, variable, value );
	}

	vector2d position_within_window( ) {
		vector2d result{ 0, 0 };
		CallVFunc<44>( 0, &result.x, &result.y );
		return result;
	}

	bool delete_async( float delay = 0.f ) {
		if ( !global::patterns::CPanel2D__DeleteAsync ) return false;
		typedef bool( __fastcall* fn_t )( CPanel2D*, float );
		return reinterpret_cast<fn_t>( global::patterns::CPanel2D__DeleteAsync )( this, delay );
	}
};

class CLabel : public CPanel2D {
public:
	std::uintptr_t set_label_text( const char* text ) {
		return CallVFunc<77>( text );
	}
	const char* label_text( ) {
		return CallVFunc<79, const char*>( );
	}
};

class CButton : public CPanel2D {
public:

};

class CSlider : public CPanel2D {
public:
	void set_fl( float vl ) {
		if ( !global::patterns::CSlider__SetValue ) return;

		reinterpret_cast<void( __fastcall* )( CSlider*, float )>( global::patterns::CSlider__SetValue )( this, vl );
	}

	int g_int( ) {
		return Member<int>( 0x4c );
	}
	float g_float( ) {
		return Member<float>( 0x4c );
	}
};

class CTextEntry : public CPanel2D {
public:
	void set_text( const char* text ) {
		if ( !global::patterns::CTextEntry__SetText ) return;
		reinterpret_cast<void( __fastcall* )( CTextEntry*, const char* )>( global::patterns::CTextEntry__SetText )( this, text );
	}

	std::string display_text( ) {
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
		const char32_t const* ptr = reinterpret_cast<const char32_t* ( __fastcall* )( CTextEntry* )>( global::patterns::CTextEntry__GetText )( this );

		return conv.to_bytes(ptr);
	}
};

class CImagePanel : public CPanel2D {
public:
	std::uintptr_t set_image_source( const char* src ) {
		if ( !global::patterns::CImagePanel__SetImage ) return 0;
		return reinterpret_cast<std::uintptr_t( __fastcall* )( CImagePanel*, const char* )>( global::patterns::CImagePanel__SetImage )( this, src );
	}
};

class CDOTA_UI_Image : public CImagePanel {
public:
	const std::string_view& source_text( ) {
		return Member<const char*>( 0x98 );
	}
};

class CDOTA_UI_AbilityImage : public CImagePanel {
public:
	void set_ability_name( const char* ability_name ) {
		if ( !global::patterns::CDOTA_UI_AbilityImage__SetAbilityName ) return;
		reinterpret_cast<void( __fastcall* )( CDOTA_UI_AbilityImage*, const char* )>( global::patterns::CDOTA_UI_AbilityImage__SetAbilityName )( this, ability_name );
	}
};

class CDOTA_UI_ItemImage : public CImagePanel {
public:
	unsigned int* set_item_name( const char* item_name ) {
		if ( !global::patterns::CDOTA_UI_ItemImage__SetItemByName ) return nullptr;
		return reinterpret_cast<unsigned int* ( __fastcall* )( CDOTA_UI_ItemImage*, const char* )>( global::patterns::CDOTA_UI_ItemImage__SetItemByName )( this, item_name );
	}
};

class C_DOTAGameManager;

class CDOTA_UI_HeroImage : public CImagePanel {
public:
	C_DOTAGameManager* set_hero_name( const char* hero_name ) {
		if ( !global::patterns::CDOTA_UI_HeroImage__SetHeroName ) return nullptr;
		return reinterpret_cast<C_DOTAGameManager * ( __fastcall* )( CDOTA_UI_HeroImage*, const char* )>( global::patterns::CDOTA_UI_HeroImage__SetHeroName )( this, hero_name );
	}
};

class CDOTA_DB_SettingsCheckbox : public CButton {
public:
	void set_selected( bool state ) {
		CallVFunc<66>( state );
	}
};

class CDropDown : public CPanel2D {
public:
	inline static CUIPanel*( *aCDropDown__GetSelected )( CDropDown* rcx ) = nullptr;

	CUIPanel* GetSelected( ) {
		return aCDropDown__GetSelected( this );
	}
};