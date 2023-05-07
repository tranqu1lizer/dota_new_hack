#pragma once

#define SOL_ALL_SAFETIES_ON 1
#define SOL_CHECK_ARGUMENTS 1

#include <mutex>
#include <filesystem>
#include "../lib/lua/sol2/sol.hpp"
#include "../../main.hpp"

#define LUA_MAXDEFCALLS 100

class CLuaScript {
private:
	sol::state script_state_;
	std::string script_name_;

	bool script_execute_;
	bool script_closing_;

	std::thread script_thread_;
	std::recursive_mutex script_mutex_;

	CLuaScript( std::string script_name );

	template <typename... Args>
	bool lua_callback( std::string funcName, Args&&... args );
public:
	inline static const std::string k_path_scripts = "C:/ResourcesDHack/scripts/";

	// CallBacks
	bool callback_on_update( );
	bool callback_on_present( );

	static CLuaScript* load_script_file( std::string script_name );
	bool reload_script( );

	std::string get_script_name( ) { return script_name_; }

};

extern std::vector<CLuaScript*> lua_scripts_list;