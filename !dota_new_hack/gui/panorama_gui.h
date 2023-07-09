#pragma once

#include "../core/sdk_game/IEngineClient.hpp"


class CPanoramaGUI {
public:
	// panels
	CUIPanel* main_panel;
	CUIPanel* main_parent_panel;

	CUIPanel* camera_dist_slider;
	CUIPanel* camera_fog_checkbox;
	CUIPanel* camera_draw_button;
	CUIPanel* changer_item_def_textentry;
	CUIPanel* changer_add_item_button;
	CUIPanel* changer_TreesDropDown;
	CUIPanel* visuals_hpnumb;
	CUIPanel* visuals_manabar;
	CUIPanel* misc_log_searching_players;
	CUIPanel* misc_autoaccept;
	CUIPanel* misc_networth_delta;
	CUIPanel* misc_unlock_emoticons;

	// settings
	bool menu_status = false;
	bool is_menu_dragging = false;

	bool draw_particles = true;
	bool draw_mana_bar = false;
	bool draw_health = false;
	bool log_searching_players = false;
	bool auto_accept = false;
	bool draw_networthdelta = false;

	void register_events();
	void show( );
};