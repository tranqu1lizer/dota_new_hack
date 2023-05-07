#include "scripts.hpp"
#include "../sdk_game/singleton.hpp"

class CLuaEvents : Singleton<CLuaEvents> {

public:

	static bool lua_on_update( );
	static bool lua_on_present( );
};