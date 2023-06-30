#pragma once

#include "../../main.hpp"

class C_DOTA_BaseNPC_Hero;

class CheatData {
public:
	std::set<C_DOTA_BaseNPC_Hero*> heroes;

	INetChannel* m_net_channel;
	vector3d traced_cursor;
	CUIPanel* ui_hud;
	bool ui_hud_status = false;
};

inline CheatData cheat_data;