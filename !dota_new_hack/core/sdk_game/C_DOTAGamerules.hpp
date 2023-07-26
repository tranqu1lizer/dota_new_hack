#pragma once

#include "../../main.hpp"
#include "../lib/protobuf/files/dota_shared_enums.pb.h"


class C_DOTAGamerules : VClass {
public:
    float GetGameTime( ) {
        if ( !util::exists( this ) ) return -0.1f;

        auto gpGlobals = (void*)CGlobalVars::get( );
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

    OFFSET( DOTA_GameState, GetGameState, schema::C_DOTAGamerules::m_nGameState );
    OFFSET( DOTA_GameMode, GetGameMode, schema::C_DOTAGamerules::m_iGameMode );
};