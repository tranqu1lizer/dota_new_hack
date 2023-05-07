#include "functions.hpp"
#include "../sdk_game/Data.hpp"

extern CheatData cheat_data;

bool functions_helper::w2s(vector3d world, vector2d* ret) {
    if (functions_call::g_pWorldToScreen == nullptr) return false;
    auto x = 0, y = 0;
    functions_call::g_pWorldToScreen(&world, &x, &y, nullptr);
    if (x > cheat_data.dotahud_actuallayoutwidth || x <= 0 || y > cheat_data.dotahud_actuallayoutheight || y <= 0) return false;

    *ret = vector2d( x, y );
    return true;
}

void functions_helper::OnColorChanged(C_DOTA_BaseNPC* npc) {
    functions_call::g_pOnColorChanged( npc );
}