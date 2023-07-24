#pragma once

#include "NormalClass.hpp"

class CGlobalVars;
class ClientClass;

enum ClientFrameStage_t : int
{
	FRAME_PRE_NET_PROCESSED, // by og
	FRAME_POST_NET_PROCESSED,// by og
	FRAME_NET_UPDATE_START = 4,
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_UPDATE_END,
};

class CSource2Client : public VClass
{
public:
	static CSource2Client* get( )
	{
		static CSource2Client* inst = nullptr;
		if ( !inst ) inst = static_cast<CSource2Client*>( util::get_interface( "client.dll", "Source2Client002" ) );

		return inst;
	}
private:
	virtual void SetGlobals( CGlobalVars* ) = 0; // 11
	virtual void FrameStageNotify( ClientFrameStage_t stage ) = 0; // 29
};

class INetChannel;

typedef void( __stdcall* VarWarningFn )( int is_in_simulation );

class CGlobalVars
{
public:
	static auto get( ) {
		return *reinterpret_cast<CGlobalVars**>( util::get_absolute_address( util::vmt( (uintptr_t)CSource2Client::get( ), 11 ) /* CSource2Client::SetGlobals() */, 3, 7 ) );
	}

	float m_realtime; // 0x0
	uint32_t m_frame_count; // 0x4
	float m_absolute_frame_time; // 0x8
	float m_frame_time; // 0xC
	int m_maxclients; // 0x10
	float m_intervalpertick; // 0x14
private:
	int unk11; // 0x18
	int unk22; // 0x1C
public:
	VarWarningFn* m_warning_func; // 0x20
private:
	float N00000081; // 0x28
public:
	float m_curtime; // 0x2C
	float m_curtime2; // 0x30
private:
	char pad[ 8 ]; // 0x34
public:
	bool m_disable_curtime_asserts; // 0x3C
	bool m_in_simulation; // 0x3D
private:
	char pad_0x0034[ 2 ]; // 0x3E
public:
	int m_tickcount; // 0x40
	float m_intervalpertick2; // 0x44
	INetChannel* m_current_netchan; // 0x48
private:
	char pad_0050[ 168 ]; // 0x50
public:
	void* m_networkstringtable; // 0xF8
private:
	char pad_0100[ 8 ]; // 0x100
public:
	void* m_clientclass_list; // 0x108
private:
	char pad_0110[ 112 ]; // 0x110
public:
	char* m_currentmap; // 0x180
	char* m_currentmap_name; // 0x188
}; // sz: 0x190

class RecvProp
{
public:
	char* m_pVarName; //0x0000 
	void* m_pSchemaClassInfo; //0x0008 
private:
	char pad_0x0010[ 0x8 ]; //0x0010
public:
	int16_t m_Offset; //0x0018 
	int16_t m_ArraySize; //0x001A 
	int32_t m_Flags; //0x001C 
private:
	char pad_0x0020[ 0x28 ]; //0x0020
public:
	void* m_pDataTable; //0x0048 
private:
	char pad_0x0050[ 0x10 ]; //0x0050
public:
	char* m_pNetName1; //0x0060 
	char* m_pNetName2; //0x0068 
	char* m_pszType; //0x0070 
private:
	char pad_0x0078[ 0x1C ]; //0x0078
public:
	int32_t m_TypeSize; //0x0094 
private:
	char pad_0x0098[ 0x8 ]; //0x0098

}; //Size=0x00A0

#pragma pack(push, 1)
class RecvPropWrapper
{
public:
	RecvProp* m_pRecvProp; //0x0000 
private:
	char pad_0x0008[ 0x7 ]; //0x0008

}; //Size=0x000F
#pragma pack(pop)

class RecvTable
{
public:
	char* m_pClassName; //0x0000 
private:
	char pad_0x0008[ 0x8 ]; //0x0008
public:
	int32_t m_nProps; //0x0010 
private:
	char pad_0x0014[ 0x4 ]; //0x0014
public:
	RecvPropWrapper* m_pProps; //0x0018 
private:
	char pad_0x0020[ 0x10 ]; //0x0020

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
		return *reinterpret_cast<IGameSystemFactory**>( Member<void*>( 0x18 ) );
	}
};