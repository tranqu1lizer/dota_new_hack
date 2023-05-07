#pragma once

#include "../../main.hpp"
#include "../sdk_game/CGCClient.hpp"

class CCommandLine;

extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

namespace hook {
	inline std::unordered_map<std::string_view, std::uintptr_t> hooks;

	bool install_hook( std::uintptr_t dwTargetAddress, LPVOID pDetour, LPVOID* ppOriginal, std::string_view name = "unknown" );
	void uninstall_hook( );

	namespace prototype {
		using RunScript_t = void( __fastcall* )( CUIEngine* thisptr, CPanel2D* panel, const char* entireJSCode, const char* pathToXMLContext, int int1, int int2, bool alreadyCompiled );
		using BAsyncSendProto_t = bool( __fastcall* )( CProtoBufMsgBase* protobufMsg, IProtoBufSendHandler* handler, google::protobuf::Message* responseMsg, unsigned int respMsgID );
		using DispatchPacket_t = bool(__fastcall*)( void*, IMsgNetPacket* );
		using PrepareUnitOrders_t = void( __fastcall* )(C_DOTAPlayerController* player, DotaUnitOrder_t order, int targetidx, vector3d* movePosition, int abilityIndex, PlayerOrderIssuer_t orderIssuer, C_DOTA_BaseNPC* entity, bool queue, bool showEffect);
		using OnAddEntity_t = CEntityInstance * (__fastcall*)(CGameEntitySystem*, C_BaseEntity*, unsigned int handle);
		using OnRemoveEntity_t = CEntityInstance * (__fastcall*)(CGameEntitySystem*, C_BaseEntity*, unsigned int handle);
		using SClientSendMessage = EGCResults( __fastcall* )( ISteamGameCoordinator* thisptr, uint32 unMsgType, const void* pubData, uint32 cubData);
		using SClientIsMessageAvailable = bool( __fastcall* )( ISteamGameCoordinator* thisptr, uint32* pcubMsgSize);
		using SClientRetrieveMessage = EGCResults( __fastcall* )( ISteamGameCoordinator* thisptr, uint32* punMsgType, void* pubDest, uint32 cubDest, uint32* pcubMsgSize);
		using RunFrame_t = void( __thiscall* )(uintptr_t);
		using FireEventClientSide_t = bool( __fastcall* )(CGameEventManager*, CGameEvent*);
		using SendNetMessage_t = bool( __fastcall* )(INetChannel* thisptr, NetMessageHandle_t*, google::protobuf::Message*, NetChannelBufType_t);
		using CreateNetChannel_t = INetChannel * (__fastcall*)(CNetworkSystem*, int, void*, const char*, unsigned int, unsigned int);
		using PostReceivedNetMessage_t = PVOID( __fastcall* )(INetChannel* rcx,
			CNetworkSerializerPB* rdx,
			google::protobuf::Message* r8,
			NetChannelBufType_t* r9);
		using Present_t = HRESULT( __fastcall* )(IDXGISwapChain*, UINT, UINT);
		using WndProc_t = LRESULT( __stdcall* )(HWND, UINT, WPARAM, LPARAM);
		using DrawBarForUnit_t = uintptr_t( __fastcall* )(uintptr_t rcx);
		using GetFogEnd_t = float( __fastcall* )(CDOTA_Camera* thisptr, float);
		using GetZFar_t = float( __fastcall* )(CDOTA_Camera* thisptr, float);
		using OnMouseWheeled_t = void( __fastcall* )(CDOTA_Camera* thisptr, int);
	}

	namespace original {
		inline prototype::RunScript_t fpRunScript;
		inline prototype::BAsyncSendProto_t fpBAsyncSendProto;
		inline prototype::DispatchPacket_t fpDispatchPacket;
		inline prototype::PrepareUnitOrders_t fpPrepareUnitOrders;
		inline prototype::OnAddEntity_t fpOnAddEntity;
		inline prototype::OnRemoveEntity_t fpOnRemoveEntity;
		inline prototype::GetFogEnd_t fpGetFogEnd;
		inline prototype::GetZFar_t fpGetZFar;
		inline prototype::OnMouseWheeled_t fpOnMouseWheeled;
		inline prototype::SClientSendMessage fpSClientSendMessage;
		inline prototype::SClientRetrieveMessage fpSClientRetrieveMessage;
		inline prototype::SClientIsMessageAvailable fpSClientIsMessageAvailable;
		inline prototype::SendNetMessage_t fpSendNetMessage;
		inline prototype::CreateNetChannel_t fpCreateNetChannel;
		inline prototype::RunFrame_t fpRunFrame;
		inline prototype::FireEventClientSide_t fpFireEventClientSide;
		inline prototype::PostReceivedNetMessage_t fpPostReceivedNetMessage;
		inline prototype::Present_t fpPresent;
		inline prototype::WndProc_t fpWndProc;
		inline prototype::DrawBarForUnit_t fpDrawBarForUnit;;
	}

	namespace functions {
		void RunScript( CUIEngine* thisptr, CPanel2D* panel, const char* entireJSCode, const char* pathToXMLContext, int int1, int int2, bool alreadyCompiled );
		bool BAsyncSendProto( CProtoBufMsgBase* protobufMsg, IProtoBufSendHandler* handler, google::protobuf::Message* responseMsg, unsigned int respMsgID );
		bool DispatchPacket( CGCClient* thisptr, IMsgNetPacket* netPacket );
		void PrepareUnitOrders( C_DOTAPlayerController* player, DotaUnitOrder_t order, int targetidx, vector3d* movePosition, int abilityIndex, PlayerOrderIssuer_t orderIssuer, C_DOTA_BaseNPC* entity, bool queue, bool showEffect );
		CEntityInstance* OnAddEntity( CGameEntitySystem* rcx, C_BaseEntity* rdx, unsigned int r8 );
		CEntityInstance* OnRemoveEntity( CGameEntitySystem* rcx, C_BaseEntity* rdx, unsigned int r8 );
		EGCResults __fastcall ISteamGameCoordinator__SendMessage( ISteamGameCoordinator* thisptr, uint32 unMsgType, ProtoBufMsgHeader_t* pubData, uint32 cubData );
		EGCResults __fastcall ISteamGameCoordinator__RetrieveMessage( ISteamGameCoordinator* thisptr, uint32* punMsgType, ProtoBufMsgHeader_t* pubDest, uint32 cubDest, uint32* pcubMsgSize );
		bool __fastcall ISteamGameCoordinator__IsMessageAvailable( ISteamGameCoordinator* thisptr, uint32* pcubMsgSize );
		bool __fastcall SendNetMessage( INetChannel* thisptr, NetMessageHandle_t* messageHandle, google::protobuf::Message* msg, NetChannelBufType_t type );
		INetChannel* __fastcall CreateNetChannel( CNetworkSystem* thisptr, int unk, void* ns_addr, const char* str, unsigned int uUnk, unsigned int uUnk2 );
		PVOID __fastcall PostReceivedNetMessage( INetChannel* rcx,
			CNetworkSerializerPB* rdx,
			google::protobuf::Message* r8,
			NetChannelBufType_t* r9 );
		void __fastcall RunFrame( uintptr_t pThis );
		bool __fastcall FireEventClientSide( CGameEventManager* thisptr, CGameEvent* ev );
		HRESULT __fastcall Present( IDXGISwapChain* pSwapchain, UINT SyncInterval, UINT Flags );
		LRESULT __stdcall WndProc( const HWND hWnd, unsigned int uMsg, uintptr_t wParam, uintptr_t lParam );
		uintptr_t __fastcall DrawBarForUnit( uintptr_t rcx );
		float __fastcall GetFogEnd( CDOTA_Camera* thisptr, float );
		float __fastcall GetZFar( CDOTA_Camera* thisptr, float );
		void __fastcall OnMouseWheeled( CDOTA_Camera* thisptr, int delta );
	}

	namespace logics {
		void ReceivingInformation( );
		void RenderInfo( );
	}
}