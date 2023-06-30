#pragma once

#include "NormalClass.hpp"

class CGlobalVars;

class CSource2Client : public VClass
{
public:
	static CSource2Client* GetInstance( )
	{
		static CSource2Client* inst = nullptr;
		if ( !inst ) inst = static_cast<CSource2Client*>( util::get_interface( "client.dll", "Source2Client002" ) );

		return inst;
	}
private:
	virtual void SetGlobals( CGlobalVars* ) = 0; // 11
	virtual void FrameStageNotify( short stage ) = 0; // 29
};

class INetChannel;

class CGlobalVars : public NormalClass
{
public:
	static auto GetInstance( ) {
		return *reinterpret_cast<CGlobalVars**>( util::get_absolute_address( util::vmt( (uintptr_t)CSource2Client::GetInstance( ), 11 ) /* CSource2Client::SetGlobals() */, 3, 7 ) );
	}

	float m_real_time; //0x0000
	uint32_t m_frame_count; //0x0004
	float m_absolute_frame_time; //0x0008
	float m_frame_time; //0x000C
private:
	char pad_0010[56]; //0x0010
public:
	INetChannel* m_current_netchan; //0x0048
private:
	char pad_0050[168]; //0x0050
public:
	void* m_networkstringtable; //0x00F8
private:
	char pad_0100[8]; //0x0100
public:
	void* m_clientclass_list; //0x0108
private:
	char pad_0110[112]; //0x0110
public:
	char* m_currentmap; //0x0180
	char* m_currentmap_name; //0x0188
};

class RecvProp
{
public:
	char* m_pVarName; //0x0000 
	void* m_pSchemaClassInfo; //0x0008 
private:
	char pad_0x0010[0x8]; //0x0010
public:
	int16_t m_Offset; //0x0018 
	int16_t m_ArraySize; //0x001A 
	int32_t m_Flags; //0x001C 
private:
	char pad_0x0020[0x28]; //0x0020
public:
	void* m_pDataTable; //0x0048 
private:
	char pad_0x0050[0x10]; //0x0050
public:
	char* m_pNetName1; //0x0060 
	char* m_pNetName2; //0x0068 
	char* m_pszType; //0x0070 
private:
	char pad_0x0078[0x1C]; //0x0078
public:
	int32_t m_TypeSize; //0x0094 
private:
	char pad_0x0098[0x8]; //0x0098

}; //Size=0x00A0

#pragma pack(push, 1)
class RecvPropWrapper
{
public:
	RecvProp* m_pRecvProp; //0x0000 
private:
	char pad_0x0008[0x7]; //0x0008

}; //Size=0x000F
#pragma pack(pop)

class RecvTable
{
public:
	char* m_pClassName; //0x0000 
private:
	char pad_0x0008[0x8]; //0x0008
public:
	int32_t m_nProps; //0x0010 
private:
	char pad_0x0014[0x4]; //0x0014
public:
	RecvPropWrapper* m_pProps; //0x0018 
private:
	char pad_0x0020[0x10]; //0x0020

}; //Size=0x0030

class ClientClass
{
public:
	const char* m_pNetworkName; // 0x0 "C_DOTA_Unit_Hero_Invoker"
	const char* m_pClassName; // 0x8 "CDOTA_Unit_Hero_Invoker"
	void* unk; // 0x10
	const char* m_pLibNameAndClassName; // 0x18 "client.dll!C_DOTA_Unit_Hero_Invoker"
	void* unk2; // 0x20;
	int m_iClassID; // 0x28;
};

class IGameSystemFactory : VClass {
public:
	IGameSystemFactory* m_next_factory;
	const char* m_name;

	void* game_system( ) {
		return CallVFunc<9, void*>( );
	}

	IGameSystemFactory* get_game_system( ) {
		return *reinterpret_cast<IGameSystemFactory**>(Member<void*>( 0x18 ));
	}
};