#pragma once

#include "../../main.hpp"

class C_DOTA_BaseNPC_Hero;

struct HeroData_t {
	//HeroData_t( C_DOTA_BaseNPC_Hero* hero, vector2d vec ) : m_hero( hero ), m_vecscreen( vec ) {}

	C_DOTA_BaseNPC_Hero* m_hero;
	//vector2d m_screen_hbo;
};

class CheatData {
public:
	std::set<C_DOTA_BaseNPC_Hero*> heroes;

	INetChannel* m_net_channel;
	CUIPanel* ui_hud;
	bool ui_hud_status = false;
};

inline CheatData cheat_data;