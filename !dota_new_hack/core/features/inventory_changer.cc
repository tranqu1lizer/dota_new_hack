#include "inventory_changer.h"

CEconItem* CInventoryChanger::create_fake_item( std::uint16_t def_index ) {
	auto& gcc = CGCClient::get( );
	if ( !( &gcc ) ) return nullptr;
	const auto dota_inventory = reinterpret_cast<CDOTAPlayerInventory*>( gcc.so_listeners( )[1] );
	if ( !( &dota_inventory ) ) return nullptr;

	CEconItem* item = dota_inventory->CreateItemObject( );
	CSOEconItem proto;
	proto.set_account_id( dota_inventory->m_soid.m_steamid & 0xFFFFFFFF );
	proto.set_inventory( ++m_invpos_counter );
	proto.set_id( ++m_itemid_counter );
	proto.set_def_index( def_index );
	proto.set_flags( 2 );
	proto.set_origin( eEconItemOrigin::kEconItemOrigin_Earned );
	item->DeserializeFromProto( &proto );

	if ( !SOCreatedForObj( item ) ) return nullptr;

	m_fake_items[item] = false;

	return item;
}

void CInventoryChanger::process_equips( CMsgClientToGCEquipItems* msg ) {
	auto& gcc = CGCClient::get( );
	if ( !( &gcc ) || !msg ) return;
	const auto dota_inventory = reinterpret_cast<CDOTAPlayerInventory*>( gcc.so_listeners( )[1] );
	if ( !( &dota_inventory ) ) return;

	for ( auto& equip : msg->equips( ) ) {
		if ( equip.has_new_slot( ) && equip.has_new_class( ) && equip.has_item_id( ) ) {
			if ( CEconItem* item = dota_inventory->find_item_by_id( equip.item_id( ) ); ( item && features::inventory_changer.m_fake_items.contains( item ) ) ) {
				if ( !m_fake_items[item] ) {
					item->un_class( ) = equip.new_class( );
					item->un_slot( ) = equip.new_slot( );
					item->flags( ) = 3;
				}
				else {
					item->un_class( ) = 0;
					item->un_slot( ) = static_cast<uint16_t>( -1 );
					item->flags( ) = 2;
				}
				if ( !SOUpdatedForObj( item ) ) return;

				m_fake_items[item] = !m_fake_items[item];
			}
			else return;
		}
	}
}

bool CInventoryChanger::process_apply_style( CMsgClientToGCSetItemStyle* style ) {
	return false;
}

void CInventoryChanger::unlock_styles_for_item( CEconItem* item ) {
	// Not implemented yet
}