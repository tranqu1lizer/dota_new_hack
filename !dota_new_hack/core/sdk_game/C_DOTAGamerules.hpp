#pragma once

#include "../../main.hpp"
#include "../lib/protobuf/files/dota_shared_enums.pb.h"


class C_DOTAGamerules : VClass {
public:
    float game_time( ) {
        if ( !util::exists( this ) ) return -0.1f;

        auto gpGlobals = (void*)CGlobalVars::GetInstance( );
        int a2 = 0;

        __int64 v2; // rdi
        float* v5; // rdx
        float* v6; // rsi
        void( __fastcall * v7 )( __int64, void* ); // rax
        float v8; // xmm1_4
        float v9; // xmm2_4
        float v10; // xmm0_4
        float* v11; // rbx
        void( __fastcall * v12 )( __int64 ); // rax
        float result; // rax
        float v14; // [rsp+30h] [rbp+8h]

        v2 = (long long)this;
        if ( !this )
        {
            v11 = (float*)gpGlobals;
            if ( !*( (BYTE*)gpGlobals + 61 ) && !*( (BYTE*)gpGlobals + 60 ) )
            {
                v12 = ( void( __fastcall* )( __int64 ) ) * ( (std::uintptr_t*)gpGlobals + 4 );
                if ( v12 )
                    v12( 1i64 );
            }
            v10 = v11[11];
            goto LABEL_17;
        }
        v5 = (float*)gpGlobals;
        v6 = (float*)gpGlobals;
        if ( !*( (BYTE*)gpGlobals + 61 ) && !*( (BYTE*)gpGlobals + 60 ) )
        {
            v7 = ( void( __fastcall* )( __int64, void* ) ) * ( (std::uintptr_t*)gpGlobals + 4 );
            if ( v7 )
            {
                v7( 1i64, gpGlobals );
                v5 = (float*)gpGlobals;
            }
        }
        v8 = v6[11];
        if ( !a2 )
        {
            if ( *(BYTE*)( v2 + 56 ) )
            {
                v9 = v5[17];
                if ( v8 >= (float)( (float)*(int*)( v2 + 52 ) * v9 ) )
                {
                    v10 = (float)( *(DWORD*)( v2 + 52 ) - *(DWORD*)( v2 + 48 ) ) * v9;
                LABEL_17:
                    v14 = v10;
                    goto LABEL_18;
                }
            }
            v8 = v8 - (float)( (float)*(int*)( v2 + 48 ) * v5[17] );
        }
        v14 = v8;
    LABEL_18:
        result = v14;
        return result;
    }

	/*float game_time( ) {
		const auto g_vars = CGlobalVars::GetInstance( );
		if ( !this ) return g_vars->m_current_time;

		if ( auto magic_time_func = g_vars->Member<some_function>( 0x20 ); magic_time_func ) {
			magic_time_func( true, g_vars );
		}
				
		const auto total_paused_ticks = Member<uint32_t>( schema::dynamic_field_offset( "client.dll/C_DOTAGamerules/m_nTotalPausedTicks" ) );
		return g_vars->m_current_time - total_paused_ticks * g_vars->m_tick_to_seconds;
	}*/

	DOTA_GameState game_state( ) {
        if ( !util::exists( this ) ) return DOTA_GAMERULES_STATE_INIT;

		static const auto offset = schema::dynamic_field_offset( "client.dll/C_DOTAGamerules/m_nGameState" );
		return Member<DOTA_GameState>( offset );
	}

	DOTA_GameMode game_mode( ) {
        if ( !util::exists( this ) ) return DOTA_GAMEMODE_NONE;

		static const auto offset = schema::dynamic_field_offset( "client.dll/C_DOTAGamerules/m_iGameMode" );
		return Member<DOTA_GameMode>( offset );
	}
};