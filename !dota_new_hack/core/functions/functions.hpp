#pragma once

#include <d3d11.h>
#include "../sdk_game/CDOTA_Camera.hpp"
#include "../sdk_game/vector.hpp"
#include "../sdk_game/color.hpp"

class C_BaseEntity;
class C_DOTA_BaseNPC;
class C_DOTABaseAbility;
class CEconItem;

inline vector2d ScreenSize;

namespace functions_prototype {
    typedef void(__fastcall* ConMsg)(const char* fmt, ...);
    typedef float(__fastcall* GetLevelSpecialValueFor_t)( C_DOTABaseAbility* ability, const char* value_name, int ability_level, std::uintptr_t* r9, std::uintptr_t* r10, std::uintptr_t* r11 );
    typedef void(__fastcall* ConColorMsg)(const Color& color, const char* format, ...);
    typedef void* (__fastcall* GetSOCDataForItem_t)(void* CDOTAPlayerInventory1, uint64_t item_id);
    typedef CEconItem*(__fastcall* CreateEconItem_t)();
    typedef bool(__fastcall* CDOTAGCClientSystem__SendReadyUpMessageForCurrentLobby)(uintptr_t, bool);
    typedef void( __fastcall* SetControlPoint_t )(uintptr_t, int, vector3d&);
    typedef void(__fastcall* CDOTARichPresence__SetRPStatus)(uintptr_t thisptr, const char* a2, int a3, __int64* a4);
    typedef CDOTA_Camera*(__fastcall* GetDOTACamera_t)();
    typedef void( __fastcall* DestroyParticle_t)(void* thisptr, unsigned int, bool);
    typedef void( __fastcall* GetGameTime_t )( float* out, bool unk );
    typedef char(__fastcall* WorldToScreen_t)(vector3d*, int*, int*, vector3d*);
    typedef void* (__fastcall* OnColorChanged_t)(C_DOTA_BaseNPC*);
    typedef uintptr_t* (__fastcall* GetPlayer_t)(int);
}

namespace functions_call {
    inline functions_prototype::ConColorMsg ConsoleColoredMessage = nullptr;
    inline functions_prototype::ConMsg ConsoleMessage = nullptr;
    inline functions_prototype::SetControlPoint_t SetControlPoint = nullptr;
    inline functions_prototype::GetLevelSpecialValueFor_t GetLevelSpecialValueFor = nullptr;
    inline functions_prototype::GetSOCDataForItem_t GetSOCDataForItem = nullptr;
    inline functions_prototype::CreateEconItem_t CreateEconItem = nullptr;
    inline functions_prototype::CDOTAGCClientSystem__SendReadyUpMessageForCurrentLobby SendReadyUpMessage = nullptr;
    inline functions_prototype::CDOTARichPresence__SetRPStatus SetRPStatus = nullptr;
    inline functions_prototype::DestroyParticle_t DestroyParticle = nullptr;
    inline functions_prototype::GetDOTACamera_t GetCurrentCamera = nullptr;
    inline functions_prototype::WorldToScreen_t g_pWorldToScreen = nullptr;
    inline functions_prototype::OnColorChanged_t g_pOnColorChanged = nullptr;
    inline functions_prototype::GetGameTime_t GetGameTime = nullptr;
    inline functions_prototype::GetPlayer_t GetPlayer = nullptr;
}

namespace functions_helper {
    bool w2s( vector3d world, vector2d* ret );
    void OnColorChanged( C_DOTA_BaseNPC* npc );
}