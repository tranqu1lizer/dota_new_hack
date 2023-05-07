#pragma once

#include "CEconItem.hpp"

class CDOTAGCClientSystem : public CSharedObjectListener {
public:
	char pad_0000[192]; // 0x0
	uint64_t m_pSteamUser; // 0xC0
};

class CDOTAPlayerInventory : public CSharedObjectListener {
public:
	SOID_t m_pSOID; // 0x8
	uint64_t m_pItems; // 0x20
	char unk_028[128]; // 0x28
	CGCClientSharedObjectCache* m_pSharedObjectCache; // 0xA0

	CEconItem* FindItem( itemid_t id ){
		if ( !this || !functions_call::GetSOCDataForItem ) return nullptr;

		return (CEconItem*)functions_call::GetSOCDataForItem( this, id );
	}
	
	CEconItem* CreateItemObject( ){
		if ( this && functions_call::CreateEconItem ) {

			if ( auto item = functions_call::CreateEconItem( ); item != nullptr ) {

				item->m_unAccountID = this->m_pSOID.m_unSteamID & 0xFFFFFFFF;
				return item;
			}
		}
		return nullptr;
	}

	virtual void vfunc2( void ) = 0;
	virtual void vfunc3( void ) = 0;
	virtual void vfunc4( void ) = 0;
	virtual void vfunc5( void ) = 0;
	virtual void vfunc6( void ) = 0;
	virtual void vfunc7( void ) = 0;
	virtual int GetMaxItemCount( ) = 0;
	virtual void vfunc9( void ) = 0;
	virtual void vfunc10( void ) = 0;
	virtual void vfunc11( void ) = 0;
	virtual void vfunc12( void ) = 0;
	virtual void vfunc13( void ) = 0;
	virtual void vfunc14( void ) = 0;
	virtual void vfunc15( void ) = 0;
	virtual __int64 SendInventoryUpdateEvent( InventoryChangeReason_t reason, item_definition_index_t definition_index, itemid_t item_id ) = 0;
	virtual void vfunc17( void ) = 0;
	virtual void vfunc18( void ) = 0;
	virtual void vfunc19( void ) = 0;
	virtual void vfunc20( void ) = 0;
	virtual void vfunc21( void ) = 0;
};

class CDOTAInventoryManager
{
public:
	CDOTAPlayerInventory* GetCDOTAPlayerInventory( ) {
		if ( !this ) return nullptr;
		return (CDOTAPlayerInventory*)(this + 0x118);
	}
};