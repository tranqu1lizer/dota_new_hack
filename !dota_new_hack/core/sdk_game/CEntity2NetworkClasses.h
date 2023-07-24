#pragma once

class IClientNetworkable {
	ClientClass* m_pClientClass; // 0x0
private:
	void* unk; // 0x8
public:
	C_BaseEntity* m_pEntity; // 0x10
private:
	void* unk2; // 0x18
public:
	char* m_pszEntityName; // 0x20
	char* m_pszClassName; // 0x28
private:
	void* unk3; // 0x30
	void* unk4; // 0x38
	void* unk5; // 0x40
	void* unk6; // 0x48
public:
	std::int32_t m_nEntity; // 0x50
};

class CEntity2NetworkClasses {
public:
	virtual ~CEntity2NetworkClasses( ) = 0; // 0
	virtual bool GetEntity2Networkable( int iIndex, IClientNetworkable* ) = 0; // 1
	virtual int GetSomeNumber( ) = 0; // 2
	virtual void* CreateEntity2Networkable( void*, void* ) = 0; // 3
	virtual void GetEntity2ClassName( IClientNetworkable*, bool ) = 0; // 4
	virtual char* const GetSomeString( ) = 0; // 5
};