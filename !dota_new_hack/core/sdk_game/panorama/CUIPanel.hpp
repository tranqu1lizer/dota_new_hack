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

	void _FindChildrenWithClassTraverse( uint16_t unClass, std::vector<CUIPanel*>& result ) {
		for ( auto panel : children( ) ) {
			if ( panel->has_class( unClass ) )
				result.push_back( panel );
			panel->_FindChildrenWithClassTraverse( unClass, result );
		}
	}

public:
	template<class T = CPanel2D>
	T* panel2d_as( ) {
		if ( !util::exists( this ) ) return nullptr;
		return Member<T*>( 0x8 );
	}
	GETTER( CUIPanel*, parent, 0x18 );
	GETTER( CUtlVector<CUIPanel*>, children, 0x28 );
	GETTER( CUtlVector<CPanoramaSymbol>, classes, 0x160 );

	CPanelStyle* panel_style( ) {
		return reinterpret_cast<CPanelStyle*>( reinterpret_cast<std::uintptr_t>( this ) + 0x70 );
	}

	bool loaded( ) {
		return CallVFunc<22, bool>( );
	}

	void SetActive( bool state ) {
		CallVFunc<31, std::uintptr_t>( state );
	}

	bool set_parent( CUIPanel* p ) {
		return CallVFunc<28>( p );
	}

	void set_draggable( bool state = false ) {
		CallVFunc<198>( state );
	}

	bool delete_async( const float delay = 0.f ) {
		if ( !global::patterns::CPanel2D__DeleteAsync ) return false;
		typedef bool( __fastcall* fn_t )( void*, float );
		return reinterpret_cast<fn_t>( global::patterns::CPanel2D__DeleteAsync )( panel2d_as<void>( ), delay );
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
		this->set_property( CPanoramaUIEngine::get( )->AccessUIEngine( )->make_symbol( symbol.data( ) ), value );
	}

	void set_style( const std::string_view& style ) {
		static constexpr auto style_symbol = "style";
		this->set_property( style_symbol, style );
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

	inline std::uintptr_t find_child_in_layout_file( const char* child )
	{
		return CallVFunc<48, bool>( child );
	}

	std::uintptr_t add_child( CUIPanel* c ) {
		return CallVFunc<257>( c );
	}

	std::uintptr_t remove_child( CUIPanel* c ) {
		return CallVFunc<260>( c );
	}

	inline std::uintptr_t load_layout_file( const char* str, bool bOverrideExisting = false )
	{
		return CallVFunc<14, bool>( str, bOverrideExisting );
	}

	inline std::uintptr_t load_layout_file_error_handle( const char* str, bool bOverrideExisting = false )
	{
		return CallVFunc<15, bool>( str, bOverrideExisting );
	}

	bool reload_layout( ) {
		return CallVFunc<268, bool>( );
	}

	inline void remove_and_delete_children( )
	{
		CallVFunc<52>( );
	}

	bool has_class( CPanoramaSymbol unClass ) {
		for ( auto& c : classes( ) )
			if ( unClass == c )
				return true;
		return false;
	}

	bool has_class( const std::string_view& cl ) {
		return has_class( CPanoramaUIEngine::get( )->AccessUIEngine( )->make_symbol( cl.data( ) ) );
	}

	void ApplyStyles( char unk ) {
		CallVFunc<70>( unk );
	}

	void AddClass( const char* class_ ) {
		const auto symbol = CPanoramaUIEngine::get( )->AccessUIEngine( )->make_symbol( class_ );
		CallVFunc<135>( symbol );
	}

	void RemoveClass(CPanoramaSymbol class_ ) {
		CallVFunc<138, void>( class_ );
	}
	
	void RemoveClass(const char* class_ ) {
		RemoveClass( CPanoramaUIEngine::get( )->AccessUIEngine( )->make_symbol( class_ ) );
	}

	void SwitchClass( const char* class_, const char* class_2 ) {
		CallVFunc<152, void>( CPanoramaUIEngine::get( )->AccessUIEngine( )->make_symbol( class_ ), CPanoramaUIEngine::get( )->AccessUIEngine( )->make_symbol( class_2 ) );
	}

	inline bool layed_out( )
	{
		return CallVFunc<345, bool>( );
	}

	[[nodiscard]]
	std::vector<CUIPanel*> find_children_with_class_traverse( uint16_t unClass ) {
		std::vector<CUIPanel*> result;
		_FindChildrenWithClassTraverse( unClass, result );
		return result;
	}

	[[nodiscard]] std::vector<CUIPanel*> find_children_traverse( const std::string_view& id ) {
		std::vector<CUIPanel*> result;
		_FindChildrenWithIdTraverse( id, result );
		return result;
	}

	[[nodiscard]] CUIPanel* find_child_traverse( const std::string_view& id ) {
		try {
			std::vector<CUIPanel*> result;
			_FindChildrenWithIdTraverse( id, result );

			return result.size( ) > 0 ? result.front( ) : nullptr;
		}
		catch ( std::exception& ex ) {
			spdlog::error( "{}\n", ex.what( ) );
			return nullptr;
		}
	}
	
	[[nodiscard]] CUIPanel* find_child_with_class_traverse( uint16_t unClass ) {
		try {
			std::vector<CUIPanel*> result;
			_FindChildrenWithClassTraverse( unClass, result );

			return result.size( ) > 0 ? result.front( ) : nullptr;
		}
		catch ( std::exception& ex ) {
			spdlog::error( "{}\n", ex.what( ) );
			return nullptr;
		}
	}

	CUIPanel* FindChild( const std::string_view& id ) {
		auto children_ = children( );

		return *std::find_if( children_.begin( ), children_.end( ), [&]( CUIPanel* v ) { return ( v->name_id( ) && v->name_id( ) == id ); } );
	}
};