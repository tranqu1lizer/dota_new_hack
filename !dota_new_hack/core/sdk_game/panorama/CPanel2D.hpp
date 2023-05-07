#pragma once

enum EDOTAPopupBackgroundStyle
{
	POPUP_BG_NONE,
	POPUP_BG_DIM,
	POPUP_BG_BLUR
};

class CPanel2D : public VClass
{
public:
	GETTER( CUIPanel*, ui_panel, 0x8 );

	vector2d position_within_window( ) {
		vector2d result{ 0, 0 };
		CallVFunc<44>( 0, &result.x, &result.y );
		return result;
	}

	void delete_async( std::uintptr_t some_arg = 0 ) {
		if ( !global::patterns::CPanel2D__DeleteAsync ) return;
		typedef std::uintptr_t( __fastcall* fn_t )( CPanel2D*, std::uintptr_t );
		reinterpret_cast<fn_t>( global::patterns::CPanel2D__DeleteAsync )( this, 0 );
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

class CTextEntry : public CPanel2D {
public:
	void set_text( const char* text ) {
		if ( !global::patterns::CTextEntry__SetText ) return;
		typedef void( __fastcall* fn_t )( CTextEntry*, const char* );
		reinterpret_cast<fn_t>( global::patterns::CTextEntry__SetText )( this, text );
	}
};

class CImagePanel : public CPanel2D {
public:
	std::uintptr_t set_image_source( const char* src ) {
		if ( !global::patterns::CImagePanel__SetImage ) return 0;
		typedef std::uintptr_t( __fastcall* fn_t )( CImagePanel*, const char* );
		return reinterpret_cast<fn_t>( global::patterns::CImagePanel__SetImage )( this, src );
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
		typedef void( __fastcall* fn_t )( CDOTA_UI_AbilityImage*, const char* );
		return reinterpret_cast<fn_t>( global::patterns::CDOTA_UI_AbilityImage__SetAbilityName )( this, ability_name );
	}
};