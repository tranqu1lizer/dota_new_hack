#pragma once

#include <windows.h>
#include <cstdint>
#include "valve/CUtlVector.hpp"
#include "CGCClient.hpp"
#include "../global.hpp"
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
class CSOEconItem;

// https://github.com/lua9520/source-engine-2018-hl2_src/blob/3bf9df6b2785fa6d951086978a3e66f49427166a/game/shared/econ/econ_item.h#L641
class CEconItem : public CGCClientSharedObject
{
public:
	void* unkx; // 0x0
	uint64_t m_ulID; // 0x10
	uint64_t unk; // 0x18
	uint32_t m_unAccountID; // 0x20
	uint32_t m_unInventory; // 0x24
	int16_t m_unDefIndex; // 0x28
	uint8_t m_nQuality;
	uint8_t m_Level;
	uint8_t m_unFlags;
	uint8_t m_unOrigin;
	char m_unStyle;

	dirty_bits_t m_dirtyBits;
	
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

	uint8_t& style( ) {
		return Field<uint8_t>( 0x30 );
	}
	uint8_t& flags( ) {
		return Field<uint8_t>( 0x31 );
	}
	uint16_t& un_class( ) {
		return Field<uint16_t>( 0x32 );
	}
	uint16_t& un_slot( ) {
		return Field<uint16_t>( 0x34 );
	}

	void DeserializeFromProto( CSOEconItem* proto ) {
		return reinterpret_cast<void ( * )( CEconItem*, CSOEconItem* )>( global::patterns::CEconItem__DeserializeItemProtobuf )( this, proto );
	}

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

enum eEconItemOrigin : std::uint32_t
{
	kEconItemOrigin_Invalid = -1,
	kEconItemOrigin_Drop = 0x0,
	kEconItemOrigin_Achievement = 0x1,
	kEconItemOrigin_Purchased = 0x2,
	kEconItemOrigin_Traded = 0x3,
	kEconItemOrigin_Crafted = 0x4,
	kEconItemOrigin_StorePromotion = 0x5,
	kEconItemOrigin_Gifted = 0x6,
	kEconItemOrigin_SupportGranted = 0x7,
	kEconItemOrigin_FoundInCrate = 0x8,
	kEconItemOrigin_Earned = 0x9,
	kEconItemOrigin_ThirdPartyPromotion = 0xa,
	kEconItemOrigin_GiftWrapped = 0xb,
	kEconItemOrigin_HalloweenDrop = 0xc,
	kEconItemOrigin_PackageItem = 0xd,
	kEconItemOrigin_Foreign = 0xe,
	kEconItemOrigin_CDKey = 0xf,
	kEconItemOrigin_CollectionReward = 0x10,
	kEconItemOrigin_PreviewItem = 0x11,
	kEconItemOrigin_SteamWorkshopContribution = 0x12,
	kEconItemOrigin_PeriodicScoreReward = 0x13,
	kEconItemOrigin_Recycling = 0x14,
	kEconItemOrigin_TournamentDrop = 0x15,
	kEconItemOrigin_PassportReward = 0x16,
	kEconItemOrigin_TutorialDrop = 0x17,
	kEconItemOrigin_RecipeOutput = 0x18,
	kEconItemOrigin_GemExtract = 0x19,
	kEconItemOrigin_EventPointReward = 0x1a,
	kEconItemOrigin_ItemRedemption = 0x1b,
	kEconItemOrigin_FantasyTicketRefund = 0x1c,
	kEconItemOrigin_VictoryPredictionReward = 0x1d,
	kEconItemOrigin_AssassinEventReward = 0x1e,
	kEconItemOrigin_CompendiumReward = 0x1f,
	kEconItemOrigin_CompendiumDrop = 0x20,
	kEconItemOrigin_MysteryItem = 0x21,
	kEconItemOrigin_UnpackedFromBundle = 0x22,
	kEconItemOrigin_WonFromWeeklyGame = 0x23,
	kEconItemOrigin_SeasonalItemGrant = 0x24,
	kEconItemOrigin_PackOpening = 0x25,
	kEconItemOrigin_InitialGrant = 0x26,
	kEconItemOrigin_MarketPurchase = 0x27,
	kEconItemOrigin_MarketRefunded = 0x28,
	kEconItemOrigin_LimitedDraft = 0x29,
	kEconItemOrigin_GauntletReward = 0x2a,
	kEconItemOrigin_CompendiumGift = 0x2b,
	kEconItemOrigin_CandyShopPurchase = 0x2c,
	kEconItemOrigin_Max = 0x2d,
};

class C_EconItemView
{
public:
	void* pad;
	item_definition_index_t m_iItemDefinitionIndex; // 0x8	
	// MNetworkEnable
	int32_t m_iEntityQuality; // 0xc	
	// MNetworkEnable
	uint32_t m_iEntityLevel; // 0x10	
private:
	[[maybe_unused]] uint8_t __pad0014[0x4]; // 0x14
public:
	// MNetworkEnable
	itemid_t m_iItemID; // 0x18	
	// MNetworkEnable
	uint32_t m_iAccountID; // 0x20	
	// MNetworkEnable
	uint32_t m_iInventoryPosition; // 0x24	
private:
	[[maybe_unused]] uint8_t __pad0028[0x8]; // 0x28
public:
	// MNetworkEnable
	bool m_bInitialized; // 0x30	
	// MNetworkEnable
	style_index_t m_nOverrideStyle; // 0x31	
	bool m_bIsStoreItem; // 0x32	
	bool m_bIsTradeItem; // 0x33	
	bool m_bHasComputedAttachedParticles; // 0x34	
	bool m_bHasAttachedParticles; // 0x35	
private:
	[[maybe_unused]] uint8_t __pad0036[0x2]; // 0x36
public:
	int32_t m_iEntityQuantity; // 0x38	
	uint8_t m_unClientFlags; // 0x3c	
private:
	[[maybe_unused]] uint8_t __pad003d[0x3]; // 0x3d
public:
	eEconItemOrigin m_unOverrideOrigin; // 0x40	
private:
	[[maybe_unused]] uint8_t __pad0044[0xc]; // 0x44
public:
	char* m_pszGrayedOutReason; // 0x50	
	// MNetworkEnable
	void* m_AttributeList; // 0x58	
};