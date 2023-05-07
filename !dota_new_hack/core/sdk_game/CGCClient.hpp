#pragma once

#include "NormalClass.hpp"

typedef int InventoryChangeReason_t;

enum ESOCacheEvent
{
	eSOCacheEvent_None = 0,
	eSOCacheEvent_Subscribed = 1,
	eSOCacheEvent_Unsubscribed = 2,
	eSOCacheEvent_Resubscribed = 3,
	eSOCacheEvent_Incremental = 4,
	eSOCacheEvent_ListenerAdded = 5,
	eSOCacheEvent_ListenerRemoved = 6,
};

struct SOID_t
{
	uint64_t m_unSteamID;
	uint32_t m_iType;
	uint32_t m_iPadding;
};

class CGCClientSharedObject : public VClass {
public:

};

class CGCClientSharedObjectCache
{
public:
	uint64_t m_pVTable; // 0x0
	char unk_8[80]; // 0x8
	// CDOTAGameAccountPlus* m_pAccountPlus; // 0x60

	virtual void unk0( void ) = 0;
	virtual void unk1( void ) = 0;
	virtual bool AddObjectToCache( CGCClientSharedObject* object ) = 0;
	virtual void unk3( void ) = 0;
	virtual void unk4( void ) = 0;
	virtual void unk5( void ) = 0;
	virtual void unk6( void ) = 0;
	virtual void unk7( void ) = 0;


};

class CSharedObjectListener {
public:
	virtual uint64_t SOCreated( SOID_t soid, CGCClientSharedObject*, ESOCacheEvent ) = 0;
	virtual uint64_t SOUpdated( SOID_t soid, CGCClientSharedObject*, ESOCacheEvent ) = 0;
};

class CGCClient : public VClass {
public:
	GETTER( CUtlVector<CSharedObjectListener*>, GetSOListeners, 0x270 );
};