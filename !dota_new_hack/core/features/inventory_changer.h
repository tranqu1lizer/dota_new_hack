#pragma once

#include "../sdk_game/CDOTA_InventoryManager.hpp"
#include <protobuf/files/econ_gcmessages.pb.h>
#include <protobuf/files/base_gcmessages.pb.h>

class CInventoryChanger {
	std::uint32_t m_itemid_counter = 0x20000000;
	std::uint32_t m_invpos_counter = 0;
public:
	std::unordered_map<CEconItem*, bool> m_fake_items;

	CEconItem* create_fake_item( std::uint16_t def_index );
	void process_equips( CMsgClientToGCEquipItems* msg );
	bool process_apply_style( CMsgClientToGCSetItemStyle* style );
	void unlock_styles_for_item( CEconItem* item );

private:
	bool SOCreatedForObj( CGCClientSharedObject* obj, ESOCacheEvent ev = eSOCacheEvent_Incremental ) {
		auto& gcc = CGCClient::get( );
		if ( !( &gcc ) ) return false;
		const auto dota_inventory = reinterpret_cast<CDOTAPlayerInventory*>( gcc.so_listeners( )[1] );
		if ( !( &dota_inventory ) ) return false;

		const auto rs = dota_inventory->SOCreated( dota_inventory->m_soid, obj, ev );
		return rs;
	}

	bool SOUpdatedForObj( CGCClientSharedObject* obj, ESOCacheEvent ev = eSOCacheEvent_Incremental ) {
		auto& gcc = CGCClient::get( );
		if ( !( &gcc ) ) return false;
		const auto dota_inventory = reinterpret_cast<CDOTAPlayerInventory*>( gcc.so_listeners( )[1] );
		if ( !( &dota_inventory ) ) return false;

		const auto rs = dota_inventory->SOUpdated( dota_inventory->m_soid, obj, ev );
		return rs;
	}
};

namespace features {
	inline CInventoryChanger inventory_changer{};
}