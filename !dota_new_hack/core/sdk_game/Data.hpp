#pragma once

#include "../../main.hpp"

class C_DOTA_BaseNPC_Hero;
class C_BaseEntity;

class CheatData {
public:
	std::set<C_DOTA_BaseNPC_Hero*> heroes;
	std::unordered_map<uint16_t, C_BaseEntity*> entities;

	INetChannel* m_net_channel;
	vector3d traced_cursor;
	CUIPanel* ui_hud;
	bool ui_hud_status = false;
};

inline CheatData context;