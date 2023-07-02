#include "tree_changer.h"

void CTreeChanger::change_trees( const std::string_view& new_tree ) {
	CDOTA_BinaryObjectSystem* binary_obj_sys = (CDOTA_BinaryObjectSystem*)util::find_game_system( "CDOTA_BinaryObjectSystem" );
	if ( !binary_obj_sys )
		return spdlog::critical( "CTreeChanger::change_trees(): binary_obj_sys = nullptr\b" );

	if ( !m_tree_model_names.contains( new_tree.data( ) ) )
		return;

	const auto should_save_orig = m_original_trees.empty( );
	for ( C_DOTA_MapTree* ent : binary_obj_sys->m_trees ) {
		if ( !ent )
			continue;

		auto game_scene = ent->schema_member<SchemaVClass*>( "client.dll/C_BaseEntity/m_pGameSceneNode" );
		if ( !game_scene )
			continue;

		if ( should_save_orig )
			m_original_trees[ ent ] = { ent->model_state( )->m_ModelName, game_scene->schema_member<float>( "client.dll/CGameSceneNode/m_flScale" ), ent->model_state( )->m_MeshGroupMask };

		ent->set_model( m_tree_model_names[ new_tree.data( ) ] );

		m_tree_model_names[ new_tree.data( ) ] == "models/props_tree/ti7/ggbranch.vmdl" ? ent->set_color( 200, 165, 0, 255 ) : ent->set_color( 255, 255, 255, 255 );

		if ( game_scene ) {
			game_scene->schema_member<float>( "client.dll/CGameSceneNode/m_flScale" ) = m_tree_models[ m_tree_model_names[ new_tree.data( ) ] ];
			game_scene->CallVFunc<10>( 4 );
		}
	}
}

void CTreeChanger::restore_trees( ) {
	for ( auto& tree : m_original_trees ) {
		if ( tree.first && tree.first->schema_member<SchemaVClass*>( "client.dll/C_BaseEntity/m_pGameSceneNode" ) ) {
			tree.first->set_model( tree.second.m_model_name );

			tree.first->set_mesh_group_mask( tree.second.m_mesh_group );
			tree.first->set_color( 255, 255, 255, 255 );
			tree.first->schema_member<SchemaVClass*>( "client.dll/C_BaseEntity/m_pGameSceneNode" )->schema_member<float>( "client.dll/CGameSceneNode/m_flScale" ) = tree.second.m_scale;
			tree.first->schema_member<SchemaVClass*>( "client.dll/C_BaseEntity/m_pGameSceneNode" )->CallVFunc<10>( 4 );
		}
	}
}