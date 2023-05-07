#pragma once

#include <fstream>

class CUIPanel;
class CPanel2D;
class CUIEventBase;
class CTopLevelWindowSource2;

typedef UtlSymId_t CPanoramaSymbol;

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

struct PanelArraySlot
{
	CUIPanel* panel;
	void* unk1;
	void* unk2;
	uint32_t someSymbol; // Might be a CPanoramaSymbol
	uint32_t boolean; // Looks to normally be 1, maybe IsVisible or IsFinishedSettingUp
};
struct PanelArray
{
	void* _pad; // all F's
	uint32_t size; // I don't think this is right.
	uint32_t unk;
	//TODO: This array is not fixed-size! Something like: CUtlVector
	//Make sure to check panel pointer when using this!
	std::array<PanelArraySlot, 4096> slots;
};

class CUIEngine : VClass
{
public:
	CStyleFactoryWrapper* GetStyleFactory( ) {
		return Member< CStyleFactoryWrapper*>( 0xD0 );
	}

	auto GetTopLevelWindows( ) const noexcept
	{
		return Member<CUtlVector<CTopLevelWindowSource2*>>( 0x60 );
	}

	PanelArray* GetPanelList( )
	{
		return Member<PanelArray*>( 0xF8 );
	}

	CPanel2D* CreatePanel( const char* symbol_str, const char* id, CUIPanel* parent )
	{
		const auto symbol = this->MakeSymbol( symbol_str );
		if ( !symbol ) return nullptr;
		return CallVFunc<29, CPanel2D*>( &symbol, id, parent );
	}

	CPanel2D* CreatePanel( CPanoramaSymbol symbol, const char* id, CUIPanel* parent )
	{
		return CallVFunc<29, CPanel2D*>( symbol, id, parent );
	}

	bool IsValidPanelPointer( CUIPanel* panel )
	{
		return CallVFunc<32, bool>( panel );
	}

	void RunScript( CUIPanel* panel, char const* script, char const* context, int unk1, int rand_value, bool already_compiled )
	{
		CallVFunc<88>( panel, script, context, unk1, rand_value, already_compiled );
	}

	CPanoramaSymbol MakeSymbol( const char* string ) const
	{
		CPanoramaSymbol result{};
		CallVFunc<121, std::uint16_t>( &result, string );
		return result;
	}

};

class CPanoramaUIEngine : IAppSystem
{
	static auto GetInstanceImpl( )
	{
		static CPanoramaUIEngine* inst = nullptr;
		if ( !inst ) inst = static_cast<CPanoramaUIEngine*>( util::get_interface( "panorama.dll", "PanoramaUIEngine001" ) );

		return inst;
	}
public:
	static auto& GetInstance( )
	{
		return *GetInstanceImpl( );
	}

	VFTable* m_pVTable; // 0x0
	uint64_t unk1; // 0x8
	uint64_t unk2; // 0x10
	uint64_t unk3; // 0x18
	CUIEngine* m_pUIEngineSource2; // 0x28

	CUIPanel* find_panel( const char* s_name ) {
		for ( const auto& current : this->m_pUIEngineSource2->GetPanelList( )->slots ) {
			auto ret = current.panel;
			if ( this->m_pUIEngineSource2->IsValidPanelPointer( ret ) ) {
				const auto name = *reinterpret_cast<const char**>( reinterpret_cast<std::uintptr_t>( ret ) + 0x10 );
				if ( name && !strcmp( name, s_name ) )
					return ret;
			}
		}
		return nullptr;
	}
};