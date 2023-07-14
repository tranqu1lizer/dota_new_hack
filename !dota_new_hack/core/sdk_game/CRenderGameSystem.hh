#pragma once

#include "IEngineClient.hpp"

inline uint32_t CountVMs( void* Interface )
{
	auto** vmt = reinterpret_cast<uintptr_t**>( Interface );

	uint32_t methodCount = 0;

	while ( vmt && ( *vmt )[ methodCount ] && IsValidCodePtr( ( *vmt )[ methodCount ] ) )
		methodCount++;

	return methodCount;
}

class CRenderGameSystem : VClass {
public:
	static CRenderGameSystem* GetInstance( ) {
		static CRenderGameSystem* inst = nullptr;
		if ( !inst )
			inst = (CRenderGameSystem*)util::find_game_system( "RenderGameSystem" );

		return inst;
	}

	auto WorldToProjectionMatrix( ) {
		static auto vft_index = 0;
		if ( !vft_index )
			vft_index = CountVMs( (void*)this ) - 1;

		return GetVF<float* ( * )( CRenderGameSystem*, int )>( vft_index )( this, 0 );
	}

	bool GetVectorInScreenSpace( const vector3d& point, vector2d& screen )
	{
		if ( !this ) return false;
		const float* worldToSurface = WorldToProjectionMatrix( );

		screen[ 0 ] = worldToSurface[ 0 ] * point[ 0 ] + worldToSurface[ 1 ] * point[ 1 ] + worldToSurface[ 2 ] * point[ 2 ] + worldToSurface[ 3 ];
		screen[ 1 ] = worldToSurface[ 4 ] * point[ 0 ] + worldToSurface[ 5 ] * point[ 1 ] + worldToSurface[ 6 ] * point[ 2 ] + worldToSurface[ 7 ];

		const float w = worldToSurface[ 12 ] * point[ 0 ] + worldToSurface[ 13 ] * point[ 1 ] + worldToSurface[ 14 ] * point[ 2 ] + worldToSurface[ 15 ];

		if ( w < 0.001f )
			return false;

		const float ww = 1.f / w;
		screen[ 0 ] *= ww;
		screen[ 1 ] *= ww;

		static int resolut[ 2 ] = { 0 };
		if ( !resolut[ 0 ] || !resolut[ 1 ] )
		{
			VClass* ss = nullptr;

			if ( ss = (VClass*)util::get_interface( "engine2.dll", "Source2EngineToClient001" ); !ss )
				return false;

			ss->CallVFunc<48, void>( &resolut[ 0 ], &resolut[ 1 ] );
		}

		screen[ 0 ] = ( (float)resolut[ 0 ] / 2.f ) + 0.5f * screen[ 0 ] * (float)resolut[ 0 ] + 0.5f;
		screen[ 1 ] = ( (float)resolut[ 1 ] / 2.f ) - 0.5f * screen[ 1 ] * (float)resolut[ 1 ] + 0.5f;

		if ( screen.x > resolut[ 0 ] || screen.y > resolut[ 1 ] )
			return false;

		return true;
	}
};