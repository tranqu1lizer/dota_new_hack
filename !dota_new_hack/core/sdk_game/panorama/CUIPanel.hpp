#pragma once

class CLabel;

class CUIPanel : public VClass
{
	void _FindChildrenWithIdTraverse( const std::string_view& id, std::vector<CUIPanel*>& result ) {
		for ( auto panel : GetChildren( ) ) {
			if ( panel->NameId( ) && panel->NameId( ) == id )
				result.push_back( panel );
			panel->_FindChildrenWithIdTraverse( id, result );
		}
	}

	void _FindChildrenWithClassTraverse( uint16_t unClass, std::vector<CUIPanel*>& result ) {
		for ( auto panel : GetChildren( ) ) {
			if ( panel->BHasClass( unClass ) )
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
	OFFSET( CUIPanel*, GetParent, 0x18 );
	OFFSET( CUtlVector<CUIPanel*>, GetChildren, 0x28 );
	OFFSET_INLINE( CPanelStyle, GetStyle, 0x70 );
	OFFSET( CUtlVector<CPanoramaSymbol>, GetClasses, 0x160 );

	void SetDialogVariable( const char* variable, int value ) {
		CallVFunc< 293, void>( variable, value );
	}

	void SetDialogVariable( const char* variable, double value ) {
		CallVFunc< 295, void>( variable, value );
	}

	void SetDialogVariable( const char* variable, const char* value ) {
		CallVFunc< 292, void>( variable, value );
	}

	bool IsLoaded( ) {
		return CallVFunc<22, bool>( );
	}

	void SetActive( bool state ) {
		CallVFunc<31, std::uintptr_t>( state );
	}

	bool SetParent( CUIPanel* p ) {
		return CallVFunc<28>( p );
	}

	bool DeleteAsync( const float delay = 0.f ) {
		if ( !global::patterns::CPanel2D__DeleteAsync ) return false;
		typedef bool( __fastcall* fn_t )( void*, float );
		return reinterpret_cast<fn_t>( global::patterns::CPanel2D__DeleteAsync )( panel2d_as<void>( ), delay );
	}

	void SetProperty( std::uint16_t symbol, const std::string_view& value ) {
		struct stAttribute
		{
			std::uint16_t _symbol{};
			const char* _value{};
		};
		stAttribute property_info{ ._symbol = symbol, ._value = value.data( ) };

		CallVFunc<304>( &property_info );
	}

	void SetProperty( const std::string_view& symbol, const std::string_view& value ) {
		SetProperty( CPanoramaUIEngine::get( )->AccessUIEngine( )->make_symbol( symbol.data( ) ), value );
	}

	void SetStyle( const std::string_view& style ) {
		static constexpr auto style_symbol = "style";
		this->SetProperty( style_symbol, style );
	}

	float actuallayoutwidth( ) {
		return CallVFunc<100, float>( );
	}

	float actuallayoutheight( ) {
		return CallVFunc<101, float>( );
	}

	inline const char* NameId( )
	{
		const auto name = *reinterpret_cast<const char**>( reinterpret_cast<std::uintptr_t>( this ) + 0x10 );
		return name ? name : "undefined";
	}

	std::uintptr_t AddChild( CUIPanel* c ) {
		return CallVFunc<257>( c );
	}

	std::uintptr_t RemoveChild( CUIPanel* c ) {
		return CallVFunc<260>( c );
	}

	inline std::uintptr_t LoadLayoutFile( const char* str, bool bOverrideExisting = false ) {
		return CallVFunc<14, bool>( str, bOverrideExisting );
	}

	inline std::uintptr_t LoadLayoutFileErrorHandle( const char* str, bool bOverrideExisting = false ) {
		return CallVFunc<15, bool>( str, bOverrideExisting );
	}

	bool BHasClass( CPanoramaSymbol unClass ) {
		for ( auto& c : GetClasses( ) )
			if ( unClass == c )
				return true;
		return false;
	}

	bool BHasClass( const std::string_view& cl ) {
		return BHasClass( CPanoramaUIEngine::get( )->AccessUIEngine( )->make_symbol( cl.data( ) ) );
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

	inline bool HasBeenLayedOut( )
	{
		return CallVFunc<345, bool>( );
	}

	[[nodiscard]]
	std::vector<CUIPanel*> FindChildrenClassTraverse( uint16_t unClass ) {
		std::vector<CUIPanel*> result;
		_FindChildrenWithClassTraverse( unClass, result );
		return result;
	}

	[[nodiscard]] std::vector<CUIPanel*> FindChildrenTraverse( const std::string_view& id ) {
		std::vector<CUIPanel*> result;
		_FindChildrenWithIdTraverse( id, result );
		return result;
	}

	[[nodiscard]] CUIPanel* FindChildTraverse( const std::string_view& id ) {
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

	CUIPanel* FindChild( const std::string_view& id ) {
		auto children_ = GetChildren( );

		return *std::find_if( children_.begin( ), children_.end( ), [&]( CUIPanel* v ) { return ( v->NameId( ) && v->NameId( ) == id ); } );
	}
};