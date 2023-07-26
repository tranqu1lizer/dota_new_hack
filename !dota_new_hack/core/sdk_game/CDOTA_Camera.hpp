#pragma once

class CDOTA_CameraManager;

class CDOTA_Camera {
public:
	OFFSET( CDOTA_CameraManager*, GetCameraManager, 0x20 );

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