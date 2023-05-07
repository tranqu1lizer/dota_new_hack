#pragma once

#include <windows.h>
#include <cstdint>
#include "valve/CUtlVector.hpp"
#include "CGCClient.hpp"

#define INVALID_EQUIPPED_SLOT ((equipped_slot_t)-1)

typedef uint16_t equipped_class_t;
typedef uint16_t equipped_slot_t;
typedef uint16_t attrib_definition_index_t;
typedef uint16_t item_definition_index_t;
typedef uint8_t	style_index_t;
typedef uint64_t itemid_t;

const uint64_t INVALID_ITEM_ID = (itemid_t)-1;

union attribute_data_union_t
{
	float asFloat;
	uint32_t asUint32;
	unsigned char* asBlobPointer;
};

struct dirty_bits_t
{
	uint8_t m_bHasEquipSingleton : 1;
	uint8_t m_bHasAttribSingleton : 1;
};

class CEconItemCustomData;

class CEconItem : public CGCClientSharedObject
{
public:
	// https://github.com/lua9520/source-engine-2018-hl2_src/blob/3bf9df6b2785fa6d951086978a3e66f49427166a/game/shared/econ/econ_item.h#L641
	void* vmt2; // 0x8
	uint64_t m_ulID; // 0x10 (Item ID)
	uint64_t unknown; // 0x18
	uint32_t m_unAccountID; // 0x20 (Account owner ID)
	uint32_t m_numInventory; // 0x24
	item_definition_index_t m_unDefIndex; // 0x28

	uint16_t& Flag( ) {
		return Member<uint16_t>( 0x30 );
	}
	uint16_t& Class( ) {
		return Member<uint16_t>( 0x32 );
	}
	uint16_t& Slot( ) {
		return Member<uint16_t>( 0x34 );
	}

	dirty_bits_t m_dirtyBits;	// dirty bits

	struct EquippedInstance_t
	{
		EquippedInstance_t( ) : m_unEquippedClass( 0 ), m_unEquippedSlot( INVALID_EQUIPPED_SLOT ) {}
		EquippedInstance_t( equipped_class_t unClass, equipped_slot_t unSlot ) : m_unEquippedClass( unClass ), m_unEquippedSlot( unSlot ) {}
		equipped_class_t m_unEquippedClass;
		equipped_slot_t m_unEquippedSlot;
	};

	struct attribute_t
	{
		attrib_definition_index_t m_unDefinitionIndex;
		attribute_data_union_t m_value;
	};

	EquippedInstance_t m_EquipInstanceSingleton; // Where the item is equipped. Valid only if m_bHasEquipSingleton and there is no custom data
	attribute_t m_CustomAttribSingleton;	// Custom attribute. Valid only if m_bHasAttribSingleton and there is no custom data

	// optional data (custom name, additional attributes, etc.)
	CEconItemCustomData* m_pCustomData;

	void EnsureCustomDataExists( );
	void UnequipFromClass( equipped_class_t unClass ) noexcept;
	void Equip( equipped_class_t unClass, equipped_slot_t unSlot );
};

class CEconItemCustomData
{
public:
	CEconItemCustomData( )
		: m_pInteriorItem( NULL )
		, m_ulOriginalID( INVALID_ITEM_ID )
		, m_unQuantity( 1 )
	{}

	~CEconItemCustomData( );

	CUtlVector< CEconItem::attribute_t > m_vecAttributes;
	CEconItem* m_pInteriorItem;
	uint64_t m_ulOriginalID;    // Original Item ID
	uint16_t m_unQuantity;    // Consumable stack count (ammo, money, etc)   

	CUtlVector<CEconItem::EquippedInstance_t> m_vecEquipped;

	// static void FreeAttributeMemory(CEconItem::attribute_t* pAttrib);
};