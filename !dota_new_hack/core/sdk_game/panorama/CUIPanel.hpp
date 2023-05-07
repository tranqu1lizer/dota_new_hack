#pragma once

class CLabel;

class CUIPanel : public VClass
{
	void _FindChildrenWithIdTraverse( const std::string_view& id, std::vector<CUIPanel*>& result ) {
		for ( auto panel : children( ) ) {
			if ( panel->name_id( ) && panel->name_id( ) == id )
				result.push_back( panel );
			panel->_FindChildrenWithIdTraverse( id, result );
		}
	}

public:
	template<class T>
	GETTER( T, panel2d_as, 0x8 );
	GETTER( CUIPanel*, parent, 0x18 );
	GETTER( CUtlVector<CUIPanel*>, children, 0x28 );
	GETTER( CUtlVector<CPanoramaSymbol>, classes, 0x160 );

	CPanelStyle* panel_style( ) {
		return reinterpret_cast<CPanelStyle*>( reinterpret_cast<std::uintptr_t>( this ) + 0x70 );
	}

	void delete_async( ) { 
		if ( !global::patterns::CPanel2D__DeleteAsync ) return;
		typedef std::uintptr_t( __fastcall* fn_t )( void* panel2d, bool remove_immediately );
		reinterpret_cast<fn_t>( global::patterns::CPanel2D__DeleteAsync )( panel2d_as<void*>( ), true );
	}

	void set_property( std::uint16_t symbol, const std::string_view& value ) {
		struct stAttribute
		{
			std::uint16_t _symbol{};
			const char* _value{};
		};
		stAttribute property_info{ ._symbol = symbol, ._value = value.data( ) };

		CallVFunc<304>( &property_info );
	}

	void set_property( const std::string_view& symbol, const std::string_view& value ) {
		this->set_property( CPanoramaUIEngine::GetInstance( ).m_pUIEngineSource2->MakeSymbol( symbol.data( ) ), value );
	}

	void set_style( const std::string_view& style ) {
		static constexpr auto sym_st = "style";
		this->set_property( sym_st, style );
	}

	const char* attribute_string( CPanoramaSymbol style_symbol, const char* ret_if_null = nullptr ) {
		return CallVFunc<279, const char*>( style_symbol, ret_if_null );
	}

	float actuallayoutwidth( ) {
		return CallVFunc<100, float>( );
	}

	float actuallayoutheight( ) {
		return CallVFunc<101, float>( );
	}

	inline const char* name_id( )
	{
		const auto name = *reinterpret_cast<const char**>( reinterpret_cast<std::uintptr_t>( this ) + 0x10 );
		return name ? name : "undefined";
	}

	CUIPanel* child_rebuild( int a2 ) {
		const auto v2 = Member<std::uintptr_t>( 0x30 );
		if ( v2 && a2 >= 0 && a2 < *reinterpret_cast<std::uintptr_t*>( v2 ) )
			return *reinterpret_cast<CUIPanel**>( v2 + 8 * a2 );
		else
			return 0;
	}

	std::uintptr_t add_child( CUIPanel* c ) {
		return CallVFunc<257>( c );
	}

	std::uintptr_t remove_child( CUIPanel* c ) {
		return CallVFunc<260>( c );
	}

	inline std::uintptr_t load_layout( const char* str, bool opt )
	{
		return CallVFunc<14, bool>( str, opt );
	}

	inline std::uintptr_t load_layout_error_handle( const char* str, bool opt )
	{
		return CallVFunc<15, bool>( str, opt );
	}

	bool reload_layout( ) {
		return CallVFunc<268, bool>(  );
	}

	inline void remove_and_delete_children( )
	{
		CallVFunc<52>( );
	}

	bool has_class( CPanoramaSymbol unClass ) {
		auto class_ = classes( );
		for ( auto& c : class_ )
			if ( unClass == c )
				return true;
		return false;
	}

	void add_class( const char* class_ ) {
		const auto symbol = CPanoramaUIEngine::GetInstance( ).m_pUIEngineSource2->MakeSymbol( class_ );
		CallVFunc<135>( symbol );
	}

	inline bool layed_out( )
	{
		return CallVFunc<345, bool>( );
	}

	[[nodiscard]] std::vector<CUIPanel*> find_children_traverse( const std::string_view& id ) {
		std::vector<CUIPanel*> result;
		_FindChildrenWithIdTraverse( id, result );
		return result;
	}

	CUIPanel* FindChild( const std::string_view& id ) {
		auto children_ = children( );

		return *std::find_if( children_.begin( ), children_.end( ), [&]( CUIPanel* v ) { return ( v->name_id( ) && v->name_id( ) == id ); } );
	}
};