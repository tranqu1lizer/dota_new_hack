#include "scripts.hpp"
#include "../lib/imgui/imgui.h"
#include "../hook/hook.hpp"

std::vector<CLuaScript*> lua_scripts_list;

extern CheatData cheat_data;

CLuaScript* CLuaScript::load_script_file( std::string script_name ) {
	return script_name.empty( ) ? nullptr : new CLuaScript( script_name );
}

CLuaScript::CLuaScript( std::string script_name ) :
	script_name_( script_name ), script_execute_( false ), script_closing_( false )
{

	const auto full_path_script = this->k_path_scripts + script_name;

	if ( !std::filesystem::exists( full_path_script ) )
	{
		std::cout << "[LUA ERROR]: File not exists\n";
		return;
	}


	script_state_.open_libraries( sol::lib::base, sol::lib::string, sol::lib::coroutine, sol::lib::package );

	script_state_.new_usertype<Color>( "Color", sol::call_constructor, sol::factories(
		[]( byte r, byte g, byte b, byte a ) { return Color{ r, g, b, a }; }
	) );
	script_state_.new_usertype <vector3d>( "Vector3D", sol::call_constructor, sol::factories(
		[]( float x, float y, float z ) { return vector3d{ x,y,z }; } ),
		"x", &vector3d::x,
		"y", &vector3d::y,
		"z", &vector3d::z,
		"Length", &vector3d::Length,
		"LengthSqr", &vector3d::LengthSqr,
		"Length2D", &vector3d::Length2D,
		"Length2DSqr", &vector3d::Length2DSqr,
		"IsZero", &vector3d::IsZero,
		"Zero", &vector3d::Zero,
		"DistTo", &vector3d::DistTo,
		"DistToSqr", &vector3d::DistToSqr,
		"Normalize", &vector3d::Normalize
		);

	script_state_.new_usertype <vector2d>( "Vector2D", sol::call_constructor, sol::factories(
		[]( float x, float y ) { return vector2d{ x,y }; } ),
		"x", &vector2d::x,
		"y", &vector2d::y,
		"Length", &vector2d::Length,
		"LengthSqr", &vector2d::LengthSqr,
		"IsZero", &vector2d::IsZero,
		"Negate", &vector2d::Negate,
		"DistTo", &vector2d::DistTo,
		"DistToSqr", &vector2d::DistToSqr
		);

	script_state_.new_usertype<C_DOTA_BaseNPC>( "CDOTA_BaseNPC",
		"GetModifierManager", &C_DOTA_BaseNPC::GetModifierManager,
		"Identity", &C_DOTA_BaseNPC::identity,
		"GetHealth", &C_DOTA_BaseNPC::health,
		"GetIndex", &C_DOTA_BaseNPC::index,
		"GetAbility", &C_DOTA_BaseNPC::GetAbility,
		"GetItem", &C_DOTA_BaseNPC::GetItem,
		"GetMaxHealth", &C_DOTA_BaseNPC::max_health,
		"GetMana", &C_DOTA_BaseNPC::mana,
		"GetMaxMana", &C_DOTA_BaseNPC::max_mana,
		"GetTeam", &C_DOTA_BaseNPC::team_number,
		"IsAlly", &C_DOTA_BaseNPC::ally,
		"SetEntityColor", &C_DOTA_BaseNPC::SetEntityColor,
		"GetAbsOrigin", &C_DOTA_BaseNPC::abs_origin,
		//"GetEntityName", &C_DOTA_BaseNPC::Identity( )::GetName( ),
		"IsHero", &C_DOTA_BaseNPC::IsHero
		);

	script_state_.set_function( "get_schema_binding_name", []( C_DOTA_BaseNPC* entity ) {
		return entity->schema_dynamic_binding( )->m_binary_name;
		} );

	script_state_.new_usertype<CDOTA_ModifierManager>( "CDOTA_ModifierManager",
		"GetModifiers", &CDOTA_ModifierManager::GetModifiers,
		"HasModifier", &CDOTA_ModifierManager::HasModifier
		);

	script_state_.new_usertype<CDOTA_ParticleManager>( "CDOTA_ParticleManager",
		"create_particle", &CDOTA_ParticleManager::CreateParticle,
		"delete_particle", sol::overload(sol::resolve<void( uint32_t )>( &CDOTA_ParticleManager::DestroyParticle ),
			sol::resolve<void( ParticleWrapper& )>( &CDOTA_ParticleManager::DestroyParticle ),
			sol::resolve<void( const char*, C_BaseEntity* )>( &CDOTA_ParticleManager::DestroyParticle ) )
		);

	script_state_.new_usertype<CNewParticleEffect>( "Particle",
		"set_control_point", &CNewParticleEffect::SetControlPoint
		);
	
	script_state_.new_usertype<EntityIndex_t>( "EntityIndex_t",
		"Get", &EntityIndex_t::Get
		);

	script_state_.new_enum<true>( "ParticleAttachment",
		"OVERHEAD_FOLLOW", ParticleAttachment_t::PATTACH_OVERHEAD_FOLLOW,
		"ABSORIGIN_FOLLOW", ParticleAttachment_t::PATTACH_ABSORIGIN_FOLLOW,
		"INVALID", ParticleAttachment_t::PATTACH_INVALID,
		"POINT", ParticleAttachment_t::PATTACH_POINT,
		"WORLDORIGIN", ParticleAttachment_t::PATTACH_WORLDORIGIN
		);

	script_state_.set_function( "world_to_screen", []( vector3d world ) {
		auto x = 0, y = 0;
		functions_call::g_pWorldToScreen( &world, &x, &y, nullptr );
		return vector2d( x, y );
								} );

	script_state_.new_enum<true>( "DotaUnitOrder",
		"HOLD_POSITION", DotaUnitOrder_t::DOTA_UNIT_ORDER_HOLD_POSITION,
		"ATTACK_TARGET", DotaUnitOrder_t::DOTA_UNIT_ORDER_ATTACK_TARGET,
		"TAUNT", DotaUnitOrder_t::DOTA_UNIT_ORDER_TAUNT,
		"CAST_NO_TARGET", DotaUnitOrder_t::DOTA_UNIT_ORDER_CAST_NO_TARGET,
		"MOVE_TO_POSITION", DotaUnitOrder_t::DOTA_UNIT_ORDER_MOVE_TO_POSITION,
		"CAST_POSITION", DotaUnitOrder_t::DOTA_UNIT_ORDER_CAST_POSITION,
		"MOVE_TO_TARGET", DotaUnitOrder_t::DOTA_UNIT_ORDER_MOVE_TO_TARGET,
		"CAST_RUNE", DotaUnitOrder_t::DOTA_UNIT_ORDER_CAST_RUNE,
		"CAST_TARGET", DotaUnitOrder_t::DOTA_UNIT_ORDER_CAST_TARGET
		);

	script_state_.new_enum<true>( "PlayerOrderIssuer",
		"HERO_ONLY", PlayerOrderIssuer_t::DOTA_ORDER_ISSUER_HERO_ONLY,
		"SELECTED_UNITS", PlayerOrderIssuer_t::DOTA_ORDER_ISSUER_SELECTED_UNITS,
		"CURRENT_UNIT_ONLY", PlayerOrderIssuer_t::DOTA_ORDER_ISSUER_CURRENT_UNIT_ONLY,
		"PASSED_UNIT_ONLY", PlayerOrderIssuer_t::DOTA_ORDER_ISSUER_PASSED_UNIT_ONLY
		);

	script_state_.set_function( "execute_order", []( C_DOTA_BaseNPC* ent, DotaUnitOrder_t order, PlayerOrderIssuer_t issuer, vector3d coord, int target_index, int ability_index, bool queue, bool effect ) {

		if ( global::g_LocalController && global::g_LocalEntity ) {
			if ( !ent ) ent = reinterpret_cast<C_DOTA_BaseNPC*>(global::g_LocalEntity);

			hook::original::fpPrepareUnitOrders(
				reinterpret_cast<C_DOTAPlayerController*>(global::g_LocalController),
				order,
				target_index,
				&coord,
				ability_index,
				issuer,
				ent,
				queue,
				effect
			);

		}
		}
	);

	script_state_.set_function( "draw_filled_rect", []( vector2d coord1, vector2d coord2, Color clr, float rnd = 0.f ) {
		ImGui::GetBackgroundDrawList( )->AddRectFilled( ImVec2{ coord1.x, coord1.y }, ImVec2{ coord2.x, coord2.y }, ImColor{ clr.RGBA[0], clr.RGBA[1], clr.RGBA[2], clr.RGBA[3] }, rnd );
		} );

	script_state_.set_function( "draw_rect", []( vector2d coord1, vector2d coord2, Color clr, float rnd = 0.f, float thck = 1.f ) {
		ImGui::GetBackgroundDrawList( )->AddRect( ImVec2{ coord1.x, coord1.y }, ImVec2{ coord2.x, coord2.y }, ImColor{ clr.RGBA[0], clr.RGBA[1], clr.RGBA[2], clr.RGBA[3] }, rnd, 0, thck );
		} );

	script_state_.set_function( "draw_circle", []( vector2d coord, Color clr, float radius, int segments = 0, float thck = 1.f ) {
		ImGui::GetBackgroundDrawList( )->AddCircle( ImVec2{ coord.x, coord.y }, radius, ImColor{ clr.RGBA[0], clr.RGBA[1], clr.RGBA[2], clr.RGBA[3] }, segments, thck );
		} );

	script_state_.set_function( "draw_text", []( vector2d coord, Color clr, std::string text ) {
		ImGui::GetBackgroundDrawList( )->AddText( ImVec2{ coord.x, coord.y }, ImColor{ clr.RGBA[0], clr.RGBA[1], clr.RGBA[2], clr.RGBA[3] }, text.c_str( ) );
		} );

	script_state_.set_function( "draw_line", []( vector2d coord1, vector2d coord2, Color clr, float thck = 1.f ) {
		ImGui::GetBackgroundDrawList( )->AddLine( ImVec2{ coord1.x, coord1.y }, ImVec2{ coord2.x, coord2.y }, ImColor{ clr.RGBA[0], clr.RGBA[1], clr.RGBA[2], clr.RGBA[3] }, thck );
		} );

	script_state_.set_function( "sleep_for", []( unsigned short ms ) {
		std::thread( [&]( ) {
			std::this_thread::sleep_for( std::chrono::milliseconds( ms ) );
			} ).detach();
		} );


	script_state_.set_function( "is_key_pressed", []( int key_code ) {
		const bool pressed = GetAsyncKeyState( key_code ) & 1;
		return pressed;
		} );

	script_state_.set_function( "is_key_down", []( int key_code ) {
		const bool down = GetAsyncKeyState( key_code );
		return down;
		} );

	script_state_.set_function( "get_local_hero", [] {
		return reinterpret_cast<C_DOTA_BaseNPC*>(global::g_LocalEntity);
		} );

	script_state_.set_function( "colored_message", []( Color clr, std::string_view str ) {
		if ( functions_call::ConsoleColoredMessage )
			functions_call::ConsoleColoredMessage( clr, str.data( ) );
		} );

	script_state_.set_function( "message", []( std::string_view str ) {
		if ( functions_call::ConsoleMessage )
			functions_call::ConsoleMessage( str.data( ) );
		} );

	script_state_.set_function( "in_game", []( ) {
		return global::g_iGameMode > EGameMode::INVALID && global::g_iGameState == EGameState::GAME_IN_PROGRESS || global::g_iGameState == EGameState::PRE_GAME;
		} );

	script_state_.set_function( "get_particle_manager", []( ) {
		return CDOTA_ParticleManager::GetInstance();
		} );

	script_state_.set_function( "get_entity_list", []( ) {
		return cheat_data.UnitList;
		} );

	// Load Lua & Ex
	const auto load_result = script_state_.safe_script_file( full_path_script, &sol::script_pass_on_error );
	if ( !load_result.valid( ) ) {
		const sol::error err = load_result;
		std::cout << "[LUA file]: " << err.what( ) << std::endl;
	}

	const auto result = load_result.get<sol::protected_function>( )();
	/*if ( !result.valid( ) ) {
		throw result.get<sol::error>( );
	}*/

	std::cout << "[LUA]: \"" + script_name + "\" loaded successfuly!\n";
}

template<typename ...Args>
bool CLuaScript::lua_callback( std::string func_name, Args && ...args ) {
	std::lock_guard<std::recursive_mutex> lock( script_mutex_ );
	if ( script_closing_ )
		return false;

	if ( script_execute_ )
		return false;

	script_execute_ = true;

	sol::protected_function func = script_state_[func_name];
	if ( !func.valid( ) ) {
		script_execute_ = false;
		return false;
	}

	sol::protected_function_result result = func( std::forward<Args>( args )... );
	if ( !result.valid( ) ) {
		const sol::error err = result;
		std::cout << "[LUA error]: " << err.what( ) << std::endl;
		script_execute_ = true;
		return false;
	}

	if ( result.get_type( ) != sol::type::boolean ) {
		script_execute_ = false;
		return false;
	}
	script_execute_ = false;
	return result.get<bool>( );
}

bool CLuaScript::callback_on_update( )
{
	return this->lua_callback( "on_update" );
}

bool CLuaScript::callback_on_present( )
{
	return this->lua_callback( "on_present" );
}