#pragma once

#include "../../main.hpp"

const enum EGameMode {
	INVALID = 0,
	ALL_PICK = 1,
	CAPTAINS_MODE = 2,
	SINGLE_DRAFT = 4,
	TRAINING = 15,
	ABILITY_DRAFT = 18,
	DEMO = 15,
	RANKED_ALL_PICK = 22,
	TURBO = 23,
};

const enum EGameState
{
	INIT,
	WAIT_FOR_PLAYERS_TO_LOAD,
	HERO_SELECTION,
	STRATEGY_TIME,
	PRE_GAME,
	GAME_IN_PROGRESS,
	POST_GAME,
	UNKNOWN_LEFT_GAME,
	UNKNOWN,
	CUSTOM_GAME_SETUP
};

class C_DOTAGamerules : VClass {
public:
	static C_DOTAGamerules* GetInstance( ) {
		if ( !global::patterns::GameRules ) return nullptr;
		return *reinterpret_cast<C_DOTAGamerules**>( global::patterns::GameRules );
	}

	float GameTime( ) {
		float result = 0;
		if ( !this || !functions_call::GetGameTime ) return result;
		functions_call::GetGameTime( &result, 0 );
		return result;
	}

	EGameState GameState( ) {
		static const auto offset = schema::dynamic_field( "client.dll/C_DOTAGamerules/m_nGameState" );
		return Member<EGameState>( offset );
	}

	EGameMode GameMode( ) {
		static const auto offset = schema::dynamic_field( "client.dll/C_DOTAGamerules/m_iGameMode" );
		return Member<EGameMode>( offset );
	}
};