#define STB_IMAGE_IMPLEMENTATION

#include <vector>
#include <string>
#include <map>
#include <format>

#include "../core/lib/imgui/stb_image.h"

#include "../core/hook/hook.hpp"

#include "gui.hpp"

ID3D11DeviceContext* CGui::pContext = NULL;
ID3D11RenderTargetView* CGui::pRenderTargetView = NULL;
bool CGui::show = true;
extern CheatData cheat_data;

#define MAX(a, b)    (((a) < (b)) ? (b) : (a))
#define INTERNAL     static

struct MultilineScrollState
{
	// Input.
	float scrollRegionX;
	float scrollX;
	ImGuiStorage* storage;

	// Output.
	bool newScrollPositionAvailable;
	float newScrollX;
};

INTERNAL int MultilineScrollCallback( ImGuiInputTextCallbackData* data )
{
	MultilineScrollState* scrollState = (MultilineScrollState*)data->UserData;
	ImGuiID cursorId = ImGui::GetID( "cursor" );
	int oldCursorIndex = scrollState->storage->GetInt( cursorId, 0 );

	if ( oldCursorIndex != data->CursorPos )
	{
		int begin = data->CursorPos;

		while ( ( begin > 0 ) && ( data->Buf[begin - 1] != '\n' ) )
		{
			--begin;
		}

		float cursorOffset = ImGui::CalcTextSize( data->Buf + begin, data->Buf + data->CursorPos ).x;
		float SCROLL_INCREMENT = scrollState->scrollRegionX * 0.25f;

		if ( cursorOffset < scrollState->scrollX )
		{
			scrollState->newScrollPositionAvailable = true;
			scrollState->newScrollX = MAX( 0.0f, cursorOffset - SCROLL_INCREMENT );
		}
		else if ( ( cursorOffset - scrollState->scrollRegionX ) >= scrollState->scrollX )
		{
			scrollState->newScrollPositionAvailable = true;
			scrollState->newScrollX = cursorOffset - scrollState->scrollRegionX + SCROLL_INCREMENT;
		}
	}

	scrollState->storage->SetInt( cursorId, data->CursorPos );

	return 0;
}

INTERNAL bool ImGuiInputTextMultiline( const char* label, char* buf, size_t buf_size, float height, ImGuiInputTextFlags flags = 0 )
{
	float scrollbarSize = ImGui::GetStyle( ).ScrollbarSize;
	float labelWidth = ImGui::CalcTextSize( label ).x + scrollbarSize;
	float SCROLL_WIDTH = 2000.0f; // Very large scrolling width to allow for very long lines.
	MultilineScrollState scrollState = {};

	// Set up child region for horizontal scrolling of the text box.
	ImGui::BeginChild( label, ImVec2( -labelWidth, height ), false, ImGuiWindowFlags_HorizontalScrollbar );
	scrollState.scrollRegionX = MAX( 0.0f, ImGui::GetWindowWidth( ) - scrollbarSize );
	scrollState.scrollX = ImGui::GetScrollX( );
	scrollState.storage = ImGui::GetStateStorage( );
	bool changed = ImGui::InputTextMultiline( label, buf, buf_size, ImVec2( SCROLL_WIDTH, MAX( 0.0f, height - scrollbarSize ) ),
											  flags | ImGuiInputTextFlags_CallbackAlways, MultilineScrollCallback, &scrollState );

	if ( scrollState.newScrollPositionAvailable )
	{
		ImGui::SetScrollX( scrollState.newScrollX );
	}

	ImGui::EndChild( );
	ImGui::SameLine( );
	ImGui::Text( label );

	return changed;
}

//https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples#Example-for-DirectX11-users
bool CGui::LoadTextureFromFile( const std::string filename, ID3D11ShaderResourceView** out_srv ) noexcept
{
	auto image_width = 0;
	auto image_height = 0;
	unsigned char* image_data = stbi_load( filename.c_str( ), &image_width, &image_height, 0, 4 );
	if ( !image_data ) {
		stbi_image_free( image_data );
		return false;
	}

	// Create texture
	D3D11_TEXTURE2D_DESC desc{};

	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture{};
	D3D11_SUBRESOURCE_DATA subResource{};
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	this->pDevice->CreateTexture2D( &desc, &subResource, &pTexture );

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	this->pDevice->CreateShaderResourceView( pTexture, &srvDesc, out_srv );
	pTexture->Release( );

	stbi_image_free( image_data );

	return true;
}

bool CGui::init( IDXGISwapChain* pSwapchain ) {
	if ( SUCCEEDED( pSwapchain->GetDevice( __uuidof( ID3D11Device ), reinterpret_cast<LPVOID*>( &this->pDevice ) ) ) ) {
		this->pDevice->GetImmediateContext( &this->pContext );

		DXGI_SWAP_CHAIN_DESC sd;
		pSwapchain->GetDesc( &sd );
		this->hWnd = sd.OutputWindow;

		ID3D11Texture2D* pBackBuffer{ nullptr };
		pSwapchain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<LPVOID*>( &pBackBuffer ) );
		if ( !pBackBuffer ) return false;

		D3D11_RENDER_TARGET_VIEW_DESC desc = {};
		memset( &desc, 0, sizeof( desc ) );

		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // most important change!
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		pDevice->CreateRenderTargetView( pBackBuffer, &desc, &this->pRenderTargetView );
		pBackBuffer->Release( );

		hook::original::fpWndProc = reinterpret_cast<hook::prototype::WndProc_t>( SetWindowLongPtrA( this->hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>( &hook::functions::WndProc ) ) );
		ImGui::CreateContext( );

		// this->ApplyStyle( );

		ImGuiIO io = ImGui::GetIO( );

		io.WantSaveIniSettings = false;
		io.IniFilename = NULL;
		io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

		this->menu_font = io.Fonts->AddFontFromFileTTF( "C:\\Windows\\Fonts\\tahomabd.ttf", 23.0f, 0, io.Fonts->GetGlyphRangesCyrillic( ) );
		this->hp_mana_font = io.Fonts->AddFontDefault( );

		ImGui_ImplWin32_Init( this->hWnd );
		ImGui_ImplDX11_Init( this->pDevice, this->pContext );
		return true;
	}
	return false;
}

void CGui::Render( ) {
	if ( !global::inited ) {
		this->pDrawList = ImGui::GetBackgroundDrawList( );
		const auto center = vector2d{cheat_data.dotahud_actuallayoutwidth, cheat_data.dotahud_actuallayoutheight } / 2.f;
		static ImColor fore_color( 200, 20, 20, 255 );
		static ImColor back_color( 200, 20, 20, 40 );
		static constexpr auto arc_size = 0.45f;
		static constexpr auto radius = 40.f;
		static constexpr auto thickness = 4.5f;

		static float position = 0.f;
		position = ImLerp( position, IM_PI * 2.f, ImGui::GetIO( ).DeltaTime * 2.3f );

		this->pDrawList->PathClear( );
		this->pDrawList->PathArcTo( ImVec2{ center.x, center.y }, radius, 0.f, 2.f * IM_PI, 40 );
		this->pDrawList->PathStroke( ImGui::GetColorU32( back_color.Value ), 0, thickness );

		this->pDrawList->PathClear( );
		this->pDrawList->PathArcTo( ImVec2{ center.x, center.y }, radius, IM_PI * 1.5f + position, IM_PI * ( 1.5f + arc_size ) + position, 40 );
		this->pDrawList->PathStroke( ImGui::GetColorU32( fore_color.Value ), 0, thickness );

		this->pDrawList->AddText( ImVec2( center.x - 88, center.y + 55 ), 0xFFFFFFFF, "Loading resources..." );

		if ( position >= IM_PI * 1.60f )
			position = 0.f;
		return;
	}
	if ( !show ) return;

	ImGui::Begin( "dota 2 super mega hacksx", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysAutoResize ); {
		if ( global::inited ) {

			if ( ImGui::CollapsingHeader( "Panorama" ) ) {
				static char buf2[128];
				static char buf3[128];
				static char buf4[128];

				ImGui::InputText( "##text2", buf2, IM_ARRAYSIZE( buf2 ) );
				if ( ImGui::Button( "Convert to string" ) ) {
					using GetStringTable_t = const char* ( __fastcall* )( std::uint16_t* a1 ); const auto callstring_table = reinterpret_cast<GetStringTable_t>( util::get_absolute_address( util::find_pattern( "panorama.dll", "E8 ? ? ? ? 48 8B D8 EB 10", "GetStringTable" ), 1, 5 ) );
					auto symbol = static_cast<uint16_t>( std::stoi( buf2 ) );
					ImGui::Text( callstring_table( &symbol ) );
				}
				ImGui::InputText( "##text3", buf3, IM_ARRAYSIZE( buf3 ) );
				if ( ImGui::Button( "Convert to symbol" ) ) {
					auto res = CPanoramaUIEngine::GetInstance( ).m_pUIEngineSource2->MakeSymbol( buf3 );
					ImGui::SameLine( );
					ImGui::Text( std::format( "{}", res ).c_str( ) );
				}
				ImGui::Separator( );
				ImGui::InputText( "##textmmr", buf4, IM_ARRAYSIZE( buf4 ) ); ImGui::SameLine( );
				if ( ImGui::Button( "Set MMR value" ) ) {
					auto& UIEngine = CPanoramaUIEngine::GetInstance( );
					auto childs = UIEngine.find_panel( "DotaDashboard" )->find_children_traverse( "MMRNumber" );
					if ( childs.size( ) ) {
						CLabel* MMRLabel = childs.front( )->panel2d_as<CLabel*>( );
						MMRLabel->set_label_text( buf4 );
					}
				}
			}

			if ( ImGui::CollapsingHeader( "Rich Presence" ) ) {
				ImGui::InputText( "Presence text: ", &this->presence_text );
				if ( ImGui::Button( "Apply text" ) ) {

					CDOTARichPresence::SetRichPresence( this->presence_text.c_str( ) );
				}
			}

			if ( ImGui::CollapsingHeader( "Maphack" ) ) {
				ImGui::Checkbox( "Particle MapHack", &this->partial_maphack );
			}

			if ( ImGui::CollapsingHeader( "AutoAccept" ) ) {
				ImGui::Checkbox( "Enabled##auto_accept", &this->auto_accept );
				if ( this->auto_accept ) {
					ImGui::Checkbox( "Accept last moment", &this->accept_in_last_moment );
					if ( !this->accept_in_last_moment )
						ImGui::SliderInt( "Accept delay (sec)", &this->auto_accept_delay, 0, 15, "%d", ImGuiSliderFlags_NoInput );
				}
			}

			if ( ImGui::CollapsingHeader( "World rendering" ) ) {
				constexpr const char* weather_list[] = { "Default", "Snow", "Rain", "Moonbeam", "Pestilence", "Ash", "Sirocco", "Snow #2", "Spring" };
				constexpr const char* river_list[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" /* "Blood", "Chrome", "Dry", "Electric", "Oil", "Potion", "Slime"*/ };

				ImGui::Checkbox( "Mouse distance change", &this->mouse_distance );
				ImGui::Checkbox( "Disable fog", &this->fog );
				if ( this->mouse_distance )
					ImGui::SliderInt( "Camera step", &this->camera_step, 10, 300 );

				if ( ImGui::Combo( "Weather", &this->current_weather, weather_list, IM_ARRAYSIZE( weather_list ) ) ) {
					int idx;
					ICVar::GetInstance( ).FindConVar( "cl_weather", idx )->value.i32 = this->current_weather;
				}

				if ( ImGui::Combo( "River type", &this->current_river, river_list, IM_ARRAYSIZE( river_list ) ) ) {
					int idx;
					auto river_type = ICVar::GetInstance( ).FindConVar( "dota_river_type", idx );
					const auto old_val = river_type->value;
					river_type->value.i32 = this->current_river;

					if ( auto cb = ICVar::GetInstance( ).GetCVarCallback( river_type->CALLBACK_INDEX ); cb )
						cb( ConVarID{ .impl = static_cast<std::uint64_t>( idx ), .var_ptr = (void*)&river_type },
							0, &river_type->value, &old_val );
				}
			}

			ImGui::Separator( );

			if ( ImGui::CollapsingHeader( "Over hero visuals" ) ) {
				ImGui::Checkbox( "Draw custom bar", &this->HackBars );
				if ( this->HackBars ) {
					ImGui::Checkbox( "Colored Bar Creeps", &this->ColoredBarCreeps );
					ImGui::Checkbox( "Draw enemy health bar", &this->HealthBars );
					if ( this->HealthBars ) {
						ImGui::Checkbox( "HP Text", &this->HealthBarsText );
					}
					ImGui::Checkbox( "Draw enemy mana bar", &this->ManaBars );
					if ( this->ManaBars ) {
						ImGui::Checkbox( "HP Text", &this->ManaBarsText );
					}
				}

				ImGui::Checkbox( "Ability panel", &this->AbilityPanel );
				if ( this->AbilityPanel ) {
					ImGui::SliderInt( "Alpha##ABIL_PANEL_ALPHA_MAIN", &this->AbilityPanelAlpha, 0, 255, "%d", ImGuiSliderFlags_NoInput );
					ImGui::SliderInt( "Size##ABIL_PANEL_SIZE_MAIN", &this->AbilityPanelSize, 0, 255, "%d", ImGuiSliderFlags_NoInput );
					ImGui::SliderInt( "Y offset##ABIL_PANEL_OFFSET_Y_MAIN", &this->AbilityPanelOffsetY, -555, 555, "%d", ImGuiSliderFlags_NoInput );
				}
			}

			if ( ImGui::CollapsingHeader( "Illusion ESP" ) ) {
				ImGui::Checkbox( "Enabled###checkbox_2", &this->illusion_esp );
				if ( this->illusion_esp ) {
					ImGui::SliderInt( "R###sliderint_1", &this->illusion_esp_r, 1, 255, "%d", ImGuiSliderFlags_NoInput );
					ImGui::SliderInt( "G###sliderint_2", &this->illusion_esp_g, 1, 255, "%d", ImGuiSliderFlags_NoInput );
					ImGui::SliderInt( "B###sliderint_3", &this->illusion_esp_b, 1, 255, "%d", ImGuiSliderFlags_NoInput );

					ImGui::Checkbox( "Dont`t select illusions", &this->illusion_esp_can_select );
				}
			}
		}
	}
	ImGui::End( );
}

void CGui::ApplyStyle( ) {
	ImGuiStyle* style = &ImGui::GetStyle( );

	style->WindowBorderSize = 0;
	style->WindowTitleAlign = ImVec2( 0.5, 0.5 );
	style->WindowMinSize = ImVec2( 600, 400 );

	style->FramePadding = ImVec2( 8, 6 );

	style->WindowPadding = ImVec2( 15, 15 );
	style->WindowRounding = 0;;
	style->FramePadding = ImVec2( 5, 5 );
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2( 12, 8 );
	style->ItemInnerSpacing = ImVec2( 8, 6 );
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 1.0f;

	style->Colors[ImGuiCol_Text] = ImVec4( 0.80f, 0.80f, 0.83f, 1.00f );
	style->Colors[ImGuiCol_TextDisabled] = ImVec4( 0.24f, 0.23f, 0.29f, 1.00f );
	style->Colors[ImGuiCol_WindowBg] = ImVec4( 0.06f, 0.05f, 0.07f, 1.00f );
	style->Colors[ImGuiCol_PopupBg] = ImVec4( 0.07f, 0.07f, 0.09f, 1.00f );
	style->Colors[ImGuiCol_Border] = ImVec4( 0.80f, 0.80f, 0.83f, 0.88f );
	style->Colors[ImGuiCol_BorderShadow] = ImVec4( 0.92f, 0.91f, 0.88f, 0.00f );
	style->Colors[ImGuiCol_FrameBg] = ImVec4( 0.10f, 0.09f, 0.12f, 1.00f );
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.24f, 0.23f, 0.29f, 1.00f );
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4( 0.56f, 0.56f, 0.58f, 1.00f );
	style->Colors[ImGuiCol_TitleBg] = ImVec4( 0.10f, 0.09f, 0.12f, 1.00f );
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4( 1.00f, 0.98f, 0.95f, 0.75f );
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4( 0.07f, 0.07f, 0.09f, 1.00f );
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4( 0.10f, 0.09f, 0.12f, 1.00f );
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4( 0.10f, 0.09f, 0.12f, 1.00f );
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4( 0.80f, 0.80f, 0.83f, 0.31f );
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.56f, 0.56f, 0.58f, 1.00f );
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( 0.06f, 0.05f, 0.07f, 1.00f );
	style->Colors[ImGuiCol_CheckMark] = ImVec4( 0.80f, 0.80f, 0.83f, 0.31f );
	style->Colors[ImGuiCol_SliderGrab] = ImVec4( 0.80f, 0.80f, 0.83f, 0.31f );
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4( 0.06f, 0.05f, 0.07f, 1.00f );
	style->Colors[ImGuiCol_Button] = ImVec4( 0.10f, 0.09f, 0.12f, 1.00f );
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4( 0.24f, 0.23f, 0.29f, 1.00f );
	style->Colors[ImGuiCol_ButtonActive] = ImVec4( 0.56f, 0.56f, 0.58f, 1.00f );
	style->Colors[ImGuiCol_Header] = ImVec4( 0.10f, 0.09f, 0.12f, 1.00f );
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4( 0.56f, 0.56f, 0.58f, 1.00f );
	style->Colors[ImGuiCol_HeaderActive] = ImVec4( 0.06f, 0.05f, 0.07f, 1.00f );
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4( 0.56f, 0.56f, 0.58f, 1.00f );
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4( 0.06f, 0.05f, 0.07f, 1.00f );
	style->Colors[ImGuiCol_PlotLines] = ImVec4( 0.40f, 0.39f, 0.38f, 0.63f );
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4( 0.25f, 1.00f, 0.00f, 1.00f );
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4( 0.40f, 0.39f, 0.38f, 0.63f );
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 0.25f, 1.00f, 0.00f, 1.00f );
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4( 0.25f, 1.00f, 0.00f, 0.43f );
}