#pragma once

#define MAX_ENTITIES_IN_LIST 512
#define MAX_ENTITY_LISTS 64 // 0x3F
#define MAX_TOTAL_ENTITIES MAX_ENTITIES_IN_LIST * MAX_ENTITY_LISTS // 0x8000

#include <cstdint>

class C_BaseEntity;

class CEntityIdentity {
public:
	C_BaseEntity* entity;
	void* dunno;
	int entHandle; // LOWORD(handle) & 0x7FFF = entID
	int unk2; // always seems to be -1
	const char* internalName; // these two strings are optional!
	const char* entityName; // ex: item_tpscroll
private:
	void* unkptr3;
public:
	char flags[4];
private:
	char pad[4];
	void* unkptr4;
	void* unkptr5;
	void* unkptr6;
	void* unkptr8;
public:
	CEntityIdentity* m_pPrev;
	CEntityIdentity* m_pNext;
	CEntityIdentity* m_pPrevByClass;
	CEntityIdentity* m_pNextByClass;
public:
	const char* entity_name( ) const {
		return internalName ? internalName : entityName;
	}
	
	const std::string_view& entity_name_view( ) const {
		return internalName ? internalName : entityName;
	}

	bool has_name( ) const {
		return ( (std::uintptr_t)entity && (std::uintptr_t)this && (internalName || entityName) );
	}

	bool dormant( ) const {
		return (this && entity) && ( flags[0] & 0x80 );
	}
};

class CEntityIdentities
{
public:
	CEntityIdentity m_identities[MAX_ENTITIES_IN_LIST];
};
