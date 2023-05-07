#pragma once

#pragma warning(disable : 4191)
#pragma warning(disable : 6387)
#pragma warning(disable : 6031)
#pragma warning(disable : 6385)
#pragma warning(disable : 6011)
#pragma warning(disable : 4099)
#pragma warning(disable : 26495)
#pragma warning(disable : 26451)
#pragma warning(disable : 26800)
#pragma warning(disable : 26813)

#include <windows.h>
#include <iostream>
#include <format>
#include <queue>
#include <string_view>
#include <vector>
#include <array>
#include <filesystem>
#include <cstddef>
#include <d3d11.h>
#include <regex>

#include <google/protobuf/text_format.h>

#include "core/lib/minhook/MinHook.h"
#pragma comment(lib, "libMinHook.x64.lib")

#include "core/sdk_game/vector.hpp"
#include "core/sdk_game/color.hpp"

#include "core/global.hpp"
#include "core/util/util.hpp"

#include "core/sdk_game/NormalClass.hpp"

#include "core/sdk_game/valve/CUtlSymbol.hpp"
#include "core/sdk_game/valve/CUtlTSHash.hpp"
#include "core/sdk_game/valve/CUtlString.hpp"
#include "core/sdk_game/valve/CUtlMemory.hpp"
#include "core/sdk_game/valve/CUtlVector.hpp"
#include "core/sdk_game/valve/CUtlRBTree.hpp"
#include "core/sdk_game/valve/CUtlMap.hpp"
#include "core/sdk_game/valve/CUtlDelegate.hpp"

#include "core/sdk_game/IAppSystem.hpp"
#include "core/sdk_game/CHandle.hpp"
#include "core/sdk_game/panorama/CPanelStyle.hpp"
#include "core/sdk_game/panorama/UIEngine.hpp"
#include "core/sdk_game/panorama/CUIPanel.hpp"
#include "core/sdk_game/panorama/CPanel2D.hpp"
#include "core/sdk_game/ISchemaSystem.hpp"
#include "core/sdk_game/IEngineClient.hpp"
#include "core/sdk_game/C_BaseEntity.hpp"
#include "core/sdk_game/CDOTA_BaseAbility.hpp"
#include "core/sdk_game/CDOTA_BaseNPC.hpp"
#include "core/sdk_game/CGameEventManager.h"
#include "core/sdk_game/CEntityInstance.hpp"
#include "core/sdk_game/CDOTA_RichPresence.hpp"
#include "core/sdk_game/ICVar.hpp"
#include "core/sdk_game/CGameEntitySystem.hpp"
#include "core/sdk_game/C_DOTAPlayerController.hpp"
#include "core/sdk_game/ISteamClient.hpp"
#include "core/sdk_game/CDOTA_ParticleManager.hpp"
#include "core/sdk_game/IResourceSystem.hpp"
#include "core/sdk_game/CGameRules.hpp"
#include "core/sdk_game/INetChan.h"
#include "core/sdk_game/DataTeamPlayer_t.hpp"
#include "core/sdk_game/valve/CBaseFileSystem.hpp"
#include "core/sdk_game/IGameUIService.hpp"
#include "core/sdk_game/Data.hpp"

#include "core/functions/functions.hpp"

#ifdef _DEBUG
#pragma comment(lib, "core/lib/protobuf/libprotobufd.lib")
#else
#pragma comment(lib, "core/lib/protobuf/libprotobuf.lib")
#endif