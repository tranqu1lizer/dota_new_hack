#pragma once

#include <cstdint>
#include <set>
#include "sdk_game/vector.hpp"
#include <map>

namespace global {
	inline auto inited = false;

	inline void* MyPanel;
	inline HMODULE resource_compiler = 0;
	inline std::string resource_path;
	inline std::uint8_t compile_step = 0;

	inline HMODULE client_module;
	inline auto camera_distance = 1200;
	inline std::map<std::string, ID3D11ShaderResourceView*> g_mapSpellIcons;
	inline std::map<std::string, ID3D11ShaderResourceView*> g_mapHeroIcons;
	inline std::map<std::string, ID3D11ShaderResourceView*> g_mapItemIcons;
	inline std::uintptr_t s_DOTAGCClientSystem;
	inline std::uintptr_t g_CGCClient;
	inline std::uintptr_t g_ColoredBarCreeps_pattern;
	inline std::uintptr_t g_GameRules_pattern;
	inline std::uint16_t g_iGameState;
	inline std::uint16_t g_iGameMode;
	inline float g_flGameTime;
	inline std::uintptr_t g_pDOTAPlayerResource;
	inline std::uintptr_t* g_LocalEntity;
	inline std::uintptr_t* g_LocalController;
	inline float RoshanKillTime;
	inline HINSTANCE hModule;

	namespace patterns {
		inline std::uintptr_t ColoredBarCreeps;
		inline std::uintptr_t GameRules;
		inline std::uintptr_t DOTAParticleManager;
		inline std::uintptr_t DOTAPlayerResource;

		inline std::uintptr_t CImagePanel__SetImage;
		inline std::uintptr_t CPanel2D__DeleteAsync;
		inline std::uintptr_t CTextEntry__SetText;
		inline std::uintptr_t CDOTA_UI_AbilityImage__SetAbilityName;
	}
}