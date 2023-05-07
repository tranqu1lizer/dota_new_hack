#include "events.hpp"

bool CLuaEvents::lua_on_update( ) {
	bool luaResult = false;
	for ( const auto& script : lua_scripts_list ) {
		if ( script ) {
			if ( script->callback_on_update( ) )
				luaResult = true;
		}
	}
	if ( luaResult )
		return true;

	return false;
}

bool CLuaEvents::lua_on_present( ) {
	bool luaResult = false;
	for ( const auto& script : lua_scripts_list ) {
		if ( script ) {
			if ( script->callback_on_present( ) )
				luaResult = true;
		}
	}
	if ( luaResult )
		return true;

	return false;
}