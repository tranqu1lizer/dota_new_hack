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

	float mat_rgba[4];

	int current_weather = 0;
	int current_river = 0;

	bool mouse_distance = true;
	int camera_step = 50;
	bool fog = true;

	bool auto_accept = true;
	bool accept_in_last_moment = false;
	int auto_accept_delay = 0;

	bool ColoredBarCreeps = true;
	// =====================
	ID3D11Device* pDevice = nullptr;
private:
	HWND hWnd = nullptr;
private:
};