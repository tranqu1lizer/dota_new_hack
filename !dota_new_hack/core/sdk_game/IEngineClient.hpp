#pragma once

#include "../../main.hpp"
#include "NormalClass.hpp"

typedef struct player_t
{
	int64_t __pad0;
	union {
		int64_t xuid;
		struct {
			int xuidlow;
			int xuidhigh;
		};
	};
	char name[128];
	int userid;
	char guid[33];
	unsigned int friendsid;
	char friendsname[128];
	bool fakeplayer;
	bool ishltv;
	unsigned int customfiles[4];
	unsigned char filesdownloaded;
} pinfo_t;

class NetChannelInfo;

class IEngineClient : VClass
{
	static auto GetInstanceImpl( )
	{
		static IEngineClient* inst = nullptr;
		if ( !inst ) inst = static_cast<IEngineClient*>( util::get_interface( "engine2.dll", "Source2EngineToClient001" ) );

		return inst;
	}
public:
	static auto& GetInstance( )
	{
		return *GetInstanceImpl( );
	}

	int GetLocalPlayerSlot( ) {
		int idx = 0;
		CallVFunc<20>( &idx, 0, 0 );
		return idx;
	}
	bool IsInGame( ) {
		return static_cast<bool>(CallVFunc<30, unsigned char>( ));
	}
};