#pragma once

#include "../main.hpp"

#include "../core/lib/imgui/imgui.h"
#include "../core/lib/imgui/imgui_impl_dx11.h"
#include "../core/lib/imgui/imgui_impl_win32.h"
#include "../core/lib/imgui/imgui_internal.h"
#include "../core/lib/imgui/imgui_stdlib.h"

class CGui
{
public:
	bool LoadTextureFromFile(const std::string, ID3D11ShaderResourceView**) noexcept;
	bool init(IDXGISwapChain* pSwapchain);
	void Render();
	HWND retWND() { return hWnd; };
private:
	void ApplyStyle();
public:
	static ID3D11DeviceContext* pContext;
	ImDrawList* pDrawList;
	static ID3D11RenderTargetView* pRenderTargetView;
	static bool show;
	ImFont* menu_font;
	ImFont* hp_mana_font;
	ImFont* def_font;

	int current_weather = 0;
	int current_river = 0;

	bool partial_maphack = true;

	bool mouse_distance = true;
	int camera_step = 50;
	bool fog = true;

	bool auto_accept = true;
	bool accept_in_last_moment = false;
	int auto_accept_delay = 0;

	std::string presence_text;

	bool HackBars = true;

	bool ColoredBarCreeps = true;

	bool ManaBars = true;
	bool ManaBarsText = true;

	bool HealthBars = true;
	bool HealthBarsText = true;
	// =====================

	bool AbilityPanel = true;
	int AbilityPanelAlpha = 255;
	int AbilityPanelSize = 30;
	int AbilityPanelOffsetY = 95;

	bool illusion_esp = false;
	bool illusion_esp_can_select = false;
	int illusion_esp_r = 0;
	int illusion_esp_g = 0;
	int illusion_esp_b = 255;

	int pos1;
	int pos2;

	ID3D11Device* pDevice = nullptr;
private:
	HWND hWnd = nullptr;
private:
};