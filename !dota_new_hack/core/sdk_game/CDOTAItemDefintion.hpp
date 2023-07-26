#include "NormalClass.hpp"

class CDOTAItemDefinition : public VClass {
public:
	// Localization strings
	OFFSET( const char*, GetName, 0x40 );
	OFFSET( const char*, GetWearableType, 0x48 );
	OFFSET( const char*, GetDesc, 0x50 );
	OFFSET( const char*, GetEnglishName, 0xf0 );

	struct CEconItemSetDefinition : public VClass {
		const char* setName, * setLocalizedName;
	};
	struct CEconStyleInfo : public VClass {
		OFFSET( const char*, GetName, 0x18 );
		OFFSET( const char*, GetModelPath, 0x20 );
	};
	struct CAssetModifierContainer : public VClass {
		OFFSET( CUtlVector<CEconStyleInfo*>*, GetStyles, 0x48 );
	};

	OFFSET( CEconItemSetDefinition*, GetItemSetDef, 0xA0 );
	OFFSET( CAssetModifierContainer*, GetAssetModifierContainer, 0xA8 );
	OFFSET( const char*, GetPrefab, 0x8 );
	OFFSET( const char*, GetHero, 0x1b0 );
	OFFSET( const char*, GetSlot, 0x1b8 );

	OFFSET( uint16_t, GetClass, 0x18C );
};

struct ItemDefNode {
	uint32_t unDefIndex, unk0;
	CDOTAItemDefinition* itemDef;
	int unk1, unk2;
};

class CDOTAItemSchema : public VClass {
public:
	inline static CDOTAItemDefinition* ( *GetItemDefByIndex )( VClass* itemSchema, uint32_t index ) = nullptr;
	inline static int( *GetItemDefArrIdx )( int* arr, uint32_t* unDefIndex ) = nullptr;

	OFFSET( ItemDefNode*, GetItemDefinitions, 0x50 );
	// Rebuilt from GetItemDefByIndex
	CDOTAItemDefinition** GetItemDefByIndexRef( uint32_t unDefIndex ) {
		auto index = GetItemDefArrIdx( MemberInline<int>( 0x20 ), &unDefIndex );
		if ( index < 0 || index >= Member<int>( 0x68 ) )
			return nullptr;
		auto itemDefs = GetItemDefinitions( );
		return itemDefs[index].unk1 < -1
			? nullptr
			: &itemDefs[index].itemDef;
	}
};