#pragma once

#include "../../main.hpp"

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

	int localplayer_index( )
	{
		int idx = -1;
		CallVFunc<26, DWORD>( &idx, 0 );
		return idx == -1 ? -1 : idx + 1;
	}

	bool in_game( ) {
		return static_cast<bool>(CallVFunc<30, unsigned char>( ));
	}

	vector2d screen_size( ) {
		int x, y;
		CallVFunc<48, void>( &x, &y );

		return vector2d{ (float)x, (float)y };
	}
};