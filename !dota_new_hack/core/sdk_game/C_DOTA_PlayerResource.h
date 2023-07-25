#pragma once

#include "valve/CUtlVector.hpp"

//enum class DOTATeam_t : int;

struct PlayerResourcePlayerData_t
{
private:
	[[maybe_unused]] uint8_t __pad0000[ 0x30 ]; // 0x0
public:
	bool m_bIsValid; // 0x30	
private:
	[[maybe_unused]] uint8_t __pad0031[ 0x7 ]; // 0x31
public:
	CUtlSymbolLarge m_iszPlayerName; // 0x38	
	int32_t m_iPlayerTeam; // 0x40	
	bool m_bFullyJoinedServer; // 0x44	
	bool m_bFakeClient; // 0x45	
	bool m_bIsBroadcaster; // 0x46	
private:
	[[maybe_unused]] uint8_t __pad0047[ 0x1 ]; // 0x47
public:
	uint32_t m_iBroadcasterChannel; // 0x48	
	uint32_t m_iBroadcasterChannelSlot; // 0x4c	
	bool m_bIsBroadcasterChannelCameraman; // 0x50	
private:
	[[maybe_unused]] uint8_t __pad0051[ 0x3 ]; // 0x51
public:
	int32_t m_iConnectionState; // 0x54	
	uint64_t m_iPlayerSteamID; // 0x58	
	int m_eCoachTeam; // 0x60	
private:
	[[maybe_unused]] uint8_t __pad0064[ 0x4 ]; // 0x64
public:
	CUtlVector<int> m_vecPrivateCoachPlayerIDs; // 0x68	
	uint32_t m_unCoachRating; // 0x80	
	int m_eLiveSpectatorTeam; // 0x84	
	int32_t m_nLiveSpectatorSpectatedHeroIndex; // 0x88	
	bool m_bIsPlusSubscriber; // 0x8c	
	bool m_bWasMVPLastGame; // 0x8d	
	std::uint8_t m_nCavernCrawlMapVariant; // 0x8e	
private:
	[[maybe_unused]] uint8_t __pad008f[ 0x1 ]; // 0x8f
public:
	int32_t m_eAccoladeType[ 3 ]; // 0x90	
private:
	[[maybe_unused]] uint8_t __pad009c[ 0x4 ]; // 0x9c
public:
	uint64_t m_unAccoladeData[ 3 ]; // 0xa0	
	int32_t m_iRankTier; // 0xb8	
	int32_t m_iTitle; // 0xbc	
	uint64_t m_unFavTeamPacked; // 0xc0	
	int m_nPlayerSlot; // 0xc8	
	bool m_bIsBot; // 0xcc	
	bool m_bHasNeutralTier[ 5 ]; // 0xcd	
	bool m_bHasRedeemedNeutralTier[ 5 ]; // 0xd2	
};

class C_DOTA_PlayerResource : VClass {
	char pad[ 0x590 ];
public:
	static C_DOTA_PlayerResource* get( ) {
		static const auto ptr = AddressWrapper( util::vmt( (uintptr_t)CSource2Client::get( ), 102 ) ).get_offset( 4 ).get_address_from_instruction_ptr( 3 );
		if ( !ptr ) return nullptr;
		return *(C_DOTA_PlayerResource**)ptr;
	}

	inline static int( *aGetNetWorthOfPlayer )( C_DOTA_PlayerResource*, unsigned int ) = nullptr;
	inline static int( *aGetLastHits )( C_DOTA_PlayerResource*, unsigned int ) = nullptr;
	inline static int( *aGetDenies )( C_DOTA_PlayerResource*, unsigned int ) = nullptr;

	int GetNetWorthOfPlayer( unsigned int playerid ) {
		if ( !this ) return -1;
		return aGetNetWorthOfPlayer( this, playerid );
	}

	int GetLastHits( unsigned int playerid ) {
		if ( !this ) return -1;
		return aGetLastHits( this, playerid );
	}

	int GetDenies( unsigned int playerid ) {
		if ( !this ) return -1;
		return aGetDenies( this, playerid );
	}

	int GetTeam( int index ) {
		if ( (int)index < 0 || (int)index >= *(DWORD*)( (std::uintptr_t)this + 0x590 ) )
			return -1;
		return *(int*)( 0xD8 * index + *(std::uintptr_t*)( (std::uintptr_t)this + 0x598 ) + 0x40 );
	}

	bool IsValidTeamPlayerID( int a2 )
	{
		int v2; // ecx
		bool result; // al

		result = 0;
		if ( a2 < 0 || a2 >= *(DWORD*)( (std::uintptr_t)this + 1344 ) )
			return result;
		v2 = *(DWORD*)( 216i64 * a2 + *(std::uintptr_t*)( (std::uintptr_t)this + 1432 ) + 64 );
		if ( (unsigned int)( v2 - 2 ) <= 1 || (unsigned int)( v2 - 6 ) <= 7 || v2 == 14 )
			return 1;
		return result;
	}

	CUtlVector< PlayerResourcePlayerData_t > m_vecPlayerData; // 0x590
private:
	char pd[ 0xC0 ];
public:
	CHandle m_playerIDToPlayer[ 64 ]; // 0x650	
	CUtlSymbolLarge m_iszName[ 64 ]; // 0x750	
	CUtlSymbolLarge m_iszHTMLSafeName[ 64 ]; // 0x950	
	CUtlSymbolLarge m_iszFilteredHTMLSafeName[ 64 ]; // 0xb50	
};