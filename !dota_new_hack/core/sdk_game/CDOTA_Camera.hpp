#pragma once

#include "../functions/functions.hpp"

class CDOTA_CameraManager;

class CDOTA_Camera {
public:
	CDOTA_CameraManager* GetCameraManager( ) {
		if ( !this ) return nullptr;

		return *reinterpret_cast<CDOTA_CameraManager**>(this + 0x20);
	}

	void SetDistance( float distance ) noexcept {
		if ( !this ) return;
		*(float*)((uintptr_t)this + 0x270) = distance;
	}

	void SetDistanceToLookAtPoint( float pos ) noexcept {
		if ( !this ) return;
		*(float*)((uintptr_t)this + 0x278) = pos;
	}

	virtual void vfunc0( void ) = 0;
	virtual void vfunc1( void ) = 0;
	virtual void vfunc2( void ) = 0;
	virtual void vfunc3( void ) = 0;
	virtual void vfunc4( void ) = 0;
	virtual void vfunc5( void ) = 0;
	virtual void vfunc6( void ) = 0;
	virtual void vfunc7( void ) = 0;
	virtual void vfunc8( void ) = 0;
	virtual void OnMouseWheeled( int delta ) = 0; // delta = 0xFFFFFFFF if zoomout, else 0x1
	virtual void vfunc10( void ) = 0;
	virtual void vfunc11( void ) = 0;
	virtual void vfunc12( void ) = 0;
	virtual void vfunc13( void ) = 0;
	virtual void vfunc14( void ) = 0;
	virtual void vfunc15( void ) = 0;
	virtual void vfunc16( void ) = 0;
	virtual void vfunc17( void ) = 0;
	virtual void vfunc18( void ) = 0;
	virtual void vfunc19( void ) = 0;
	virtual void vfunc20( void ) = 0;
	virtual bool ShouldHideCursor( ) = 0;
};

class CDOTA_CameraManager {
public:

};