#pragma once

#include "../sdk_game/C_DOTA_MapTree.hpp"

class CTreeChanger {
	struct original_tree_t {
		const char* m_model_name;
		float m_scale;
		uint64_t m_mesh_group;
	};

	std::unordered_map<C_DOTA_MapTree*, original_tree_t> m_original_trees;

	std::unordered_map<std::string, float> m_tree_models = std::unordered_map<std::string, float>( {
		{ "models/props_structures/crystal003_refract.vmdl", 1} ,
		{ "models/props_structures/pumpkin001.vmdl", 1.08 },
		{ "models/props_structures/pumpkin003.vmdl", 3 },
		{ "models/props_diretide/pumpkin_head.vmdl", 3 },
		{ "models/props_gameplay/pumpkin_bucket.vmdl", 1 },
		{ "maps/jungle_assets/trees/pitcher/jungle_pitcher_tree.vmdl", 2 },
		{ "models/props_garden/tree_garden001.vmdl", 1.3 },
		{ "maps/journey_assets/props/trees/journey_armandpine/journey_armandpine_02_stump.vmdl", 4.5 },
		{ "models/props_tree/frostivus_tree.vmdl", 0.85 },
		{ "models/props_tree/ti7/ggbranch.vmdl", 1 },
		{ "models/props_tree/newbloom_tree.vmdl", 0.9 },
	} );

	std::unordered_map<std::string, std::string> m_tree_model_names = std::unordered_map<std::string, std::string>( {
		{ "Crystal", "models/props_structures/crystal003_refract.vmdl"} ,
		{ "Pumpkins #1", "models/props_structures/pumpkin001.vmdl"},
		{ "Pumpkins #2", "models/props_structures/pumpkin003.vmdl"},
		{ "Pumpkins #3","models/props_diretide/pumpkin_head.vmdl"},
		{ "Pumpkin Buckets","models/props_gameplay/pumpkin_bucket.vmdl" },
		{ "Jungle pitcher", "maps/jungle_assets/trees/pitcher/jungle_pitcher_tree.vmdl" },
		{ "Tree garden", "models/props_garden/tree_garden001.vmdl" },
		{ "Stumps", "maps/journey_assets/props/trees/journey_armandpine/journey_armandpine_02_stump.vmdl" },
		{ "Frostivus", "models/props_tree/frostivus_tree.vmdl" },
		{ "TI7 GG Tree", "models/props_tree/ti7/ggbranch.vmdl" },
		{ "New Bloom", "models/props_tree/newbloom_tree.vmdl" },
	} );
public:
	void change_trees( const std::string_view& new_tree );
	void restore_trees( );
};


namespace features {
	inline CTreeChanger tree_changer{};
}