#pragma once

class CDOTA_CameraManager;

class CDOTA_Camera {
public:
	CDOTA_CameraManager* GetCameraManager( ) {
		if ( !this ) return nullptr;

		return *reinterpret_cast<CDOTA_CameraManager**>( this + 0x20 );
	}

	void set_distance( float distance ) noexcept {
		if ( !this ) return;
		*(float*)( (uintptr_t)this + 0x270 ) = distance;
	}

	void SetDistanceToLookAtPoint( float pos ) noexcept {
		if ( !this ) return;
		*(float*)( (uintptr_t)this + 0x278 ) = pos;
	}
};

class CDOTA_CameraManager {
public:

};