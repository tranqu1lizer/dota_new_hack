#pragma once

#include <d3d11.h>
#include "../sdk_game/vector.hpp"
#include "../sdk_game/color.hpp"
#include "../lib/protobuf/protobuf/google/protobuf/message_lite.h"

class C_DOTABaseAbility;
class CDOTAPlayerInventory;
class CDOTA_Camera;
class CEconItem;
class CPanel2D;

namespace calls {
	inline float( *GetLevelSpecialValueFor )( C_DOTABaseAbility* ability, const char* value_name, int ability_level, std::uintptr_t* r9, std::uintptr_t* r10, std::uintptr_t* r11 ) = nullptr;
	inline void* ( *GetSOCDataForItem )( CDOTAPlayerInventory*, uint64_t item_id ) = nullptr;
	inline CEconItem* ( *CreateEconItemObject )( ) = nullptr;
	inline void( *destroy_particle )( void* thisptr, unsigned int, bool ) = nullptr;
	inline CDOTA_Camera* ( *GetCurrentCamera )( ) = nullptr;
	inline void( *CDOTA_Hud_Top_Bar__UpdateNetWorthDifference )( CPanel2D*, int RadiantNetWorth, int DireNetWorth ) = nullptr;
}