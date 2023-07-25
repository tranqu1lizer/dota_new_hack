#pragma once

#include "../../main.hpp"

using CSplitScreenSlot = unsigned int;

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
	DEFINE_INTERFACE( IEngineClient, "engine2.dll", "Source2EngineToClient001" );

	int localplayer_index( )
	{
		int idx = -1;
		CallVFunc<26, DWORD>( &idx, 0 );
		return idx == -1 ? -1 : idx + 1;
	}

	bool IsInGame( ) {
		return static_cast<bool>(CallVFunc<30, unsigned char>( ));
	}

	vector2d screen_size( ) {
		int x, y;
		CallVFunc<48, void>( &x, &y );

		return vector2d{ (float)x, (float)y };
	}
};