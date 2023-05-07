#pragma once

#include "../../main.hpp"

enum EAbilityType : int {
	Invoker_Sun_Strike,
	Lina_Light_Strike_Array,
	Kunkka_Torrent
};

struct InvetoryEntity_t {
	C_DOTA_Item* Item;
	float ManaCost;
	float Cooldown;
	float MaxCooldown;
};

struct ParticleData_t {
	int particle_idx = 0;
	uintptr_t name_idx = 0;
	std::string resource_name = "";
	C_BaseEntity* attached_entity = nullptr;
	vector3d position = { 0, 0, 0 };
	float create_time = 0.f;
};

struct ProjectileData_t {
	int entity_idx = 0;
	int projectile_handle = 0;
	C_BaseEntity* attached_entity = nullptr;
	vector3d position = { 0,0,0 };
	float create_time = 0.f;
};

struct AbilityData_t
{
	EAbilityType ability;
	C_BaseEntity* ability_ent = nullptr;
	float create_time = 0.f;
};

class CheatData {
public:
	float dotahud_actuallayoutwidth;
	float dotahud_actuallayoutheight;
	bool m_bCameraHooked = false;
	float m_flRoshanSpawned = -1;
	std::set<C_DOTA_BaseNPC*> UnitList;
	std::map<std::uint16_t, ParticleData_t> PARTICLE_LIST;
	std::map<std::uint16_t, ParticleData_t>::iterator PARTICLE_LIST_ITERATOR;
	std::map<std::uint32_t, ProjectileData_t> PROJECTILE_LIST;
	std::map<std::uint32_t, ProjectileData_t>::iterator PROJECTILE_LIST_ITERATOR;
	std::vector<AbilityData_t> ABILITYESP_LIST;
	std::vector<AbilityData_t>::iterator ABILITYESP_LIST_ITERATOR;
	std::vector<C_DOTA_BaseNPC_Hero*> m_hero_list;
	std::unordered_map<C_DOTA_BaseNPC_Hero*, CUIPanel*> m_hero_panels;

	INetChannel* m_pNetChannel;
};