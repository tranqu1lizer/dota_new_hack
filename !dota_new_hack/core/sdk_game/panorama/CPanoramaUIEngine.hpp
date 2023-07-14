#pragma once

#include <fstream>

class CUIPanel;
class CPanel2D;
class CUIEventBase;
class CTopLevelWindowSource2;
class CUIRenderDeviceSource2;
class CUIEngineSource2;

typedef UtlSymId_t CPanoramaSymbol;

class PanelListNode {
	void* unk0, * unk1;
public:
	CUIPanel* uiPanel;
private:
	void* unk2;
};

struct PanelHandle_t
{
	DWORD m_low,
		m_high;

	PanelHandle_t( ) {}

	explicit PanelHandle_t( DWORD low, DWORD high ) : m_low( low ), m_high( high )
	{
	}

	static PanelHandle_t InvalidHandle( )
	{
		static PanelHandle_t s_invalid( 0, -1 );
		return s_invalid;
	}
};

class CPanoramaUIEngine : IAppSystem
{
public:
	static auto get( )
	{
		static CPanoramaUIEngine* inst = nullptr;
		if ( !inst ) inst = static_cast<CPanoramaUIEngine*>( util::get_interface( "panorama.dll", "PanoramaUIEngine001" ) );

		return inst;
	}

	CUIEngineSource2* engine_source2( ) {
		return *(CUIEngineSource2**)( (std::uintptr_t)this + 0x28 );
	}
};

class CUIEngineSource2 : VClass
{
public:
	CStyleFactoryWrapper* GetStyleFactory( ) {
		return Member<CStyleFactoryWrapper*>( 0xD0 );
	}

	CLayoutManager* GetLayoutManager( ) {
		return Member<CLayoutManager*>( 0x160 );
	}

	auto GetTopLevelWindows( ) const noexcept {
		return Member<CUtlVector<CTopLevelWindowSource2*>>( 0x60 );
	}

	CUIRenderDeviceSource2* render_device( ) {
		return Member<CUIRenderDeviceSource2*>( 0xBA8 );
	}

	template<uint32_t size>
	auto panel_list( ) {
		return std::span{ Member<PanelListNode*>( 0xf8 ), size };
	}

	CPanel2D* create_panel( const char* id, const char* parent_name ) {
		const auto symbol = this->make_symbol( "Panel" );
		auto parent = this->find_panel( parent_name );
		if ( !symbol || !parent ) return nullptr;
		return CallVFunc<29, CPanel2D*>( symbol, id, parent );
	}

	CPanel2D* create_panel( const char* id, CUIPanel* parent ) {
		const auto symbol = this->make_symbol( "Panel" );
		if ( !symbol || !parent ) return nullptr;
		return CallVFunc<29, CPanel2D*>( symbol, id, parent );
	}

	CPanel2D* create_panel( CPanoramaSymbol symbol, const char* id, CUIPanel* parent ) {
		return CallVFunc<29, CPanel2D*>( symbol, id, parent );
	}

	CUIPanel* GetPanelPtr( PanelHandle_t& handle )
	{
		return CallVFunc<34, CUIPanel*>( handle );
	}

	void CreateDebuggerWindow( ) {
		CallVFunc<116>( );
	}

	void CloseDebuggerWindow( ) {
		CallVFunc<117>( );
	}

	bool is_valid_panel_ptr( CUIPanel* panel ) {
		return CallVFunc<32, bool>( panel );
	}

	void RunScript( CUIPanel* panel, char const* script, char const* context ) {
		CallVFunc<88>( panel, script, context, 0, 0, 0 );
	}

	void RunScript( const char* pname, char const* script, char const* context ) {
		RunScript( find_panel( pname ), script, context );
	}

	void ExecuteScript( char const* s ) {
		RunScript( "Hud", s, global::in_game ? "panorama/layout/base_hud.xml" : "panorama/layout/base.xml" );
	}

	void play_sound_effect( const std::string_view& ev_name ) {
		const auto str = std::format( "$.DispatchEvent( 'PlaySoundEffect', '{}' );", ev_name );
		this->RunScript(
			this->find_panel( "Hud" ),
			str.data( ),
			global::in_game ? "panorama/layout/base_hud.xml" : "panorama/layout/base.xml"
		);
	}

	CPanoramaSymbol make_symbol( const char* string ) {
		CPanoramaSymbol result{};
		CallVFunc<121, std::uint16_t>( &result, string );
		return result;
	}

	void register_event_handler_client( CPanoramaSymbol event, CUIPanel* panel, CUtlAbstractDelegate handler ) {
		CallVFunc<40, void>( event, panel, handler );
	}

	void register_event_handler_client( const std::string_view& str, CUIPanel* panel, FastDelegate0<void> handler ) {
		register_event_handler_client( CPanoramaUIEngine::get( )->engine_source2( )->make_symbol( str.data( ) ), panel, handler.GetAbstractDelegate( ) );
	}

	void unregister_event_handler( CPanoramaSymbol event, CUIPanel* panel, CUtlAbstractDelegate handler ) {
		CallVFunc<42, void>( event, panel, handler );
	}

	void unregister_event_handler( const std::string_view& str, CUIPanel* panel, FastDelegate0<void> handler ) {
		unregister_event_handler( CPanoramaUIEngine::get( )->engine_source2( )->make_symbol( str.data( ) ), panel, handler.GetAbstractDelegate( ) );
	}

	CUIPanel* find_panel( const char* s_name ) {
		for ( const auto& current : this->panel_list<4096>( ) ) {
			auto ret = current.uiPanel;
			if ( this->is_valid_panel_ptr( ret ) ) {
				const auto name = *reinterpret_cast<const char**>( reinterpret_cast<std::uintptr_t>( ret ) + 0x10 );
				if ( name && !util::fast_strcmp( (char*)name, (char*)s_name ) )
					return ret;
			}
		}
		return nullptr;
	}
};