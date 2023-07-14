#pragma once

#include "../../main.hpp"
#include "../sdk_game/CGCClient.hpp"

extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

namespace hook {
	inline std::unordered_map<std::uintptr_t, std::string_view> hooks;

	bool install_hook( std::uintptr_t dwTargetAddress, LPVOID pDetour, LPVOID* ppOriginal, std::string_view name );
	bool uninstall_hook( std::uintptr_t address );
	void uninstall_all_hooks( );

	namespace original {
		inline void* fpCreateMove;
		inline void* fpGetMaterialForDraw;
		inline void* fpBAsyncSendProto;
		inline void* fpOnMouseWheeled;
		inline void* fpSGCRetrieveMessage;
		inline void* fpSendNetMessage;
		inline void* fpPostReceivedNetMessage;
		inline void* fpPresent;
		inline void* fpWndProc;
	}

	namespace functions {
		IMaterial2* GetMaterialForDraw( CBaseSceneObjectDesc* rcx, IMaterial2* rdx, CMaterialDrawDescriptor* r8, ISceneLayer* r9, bool& r10 );
		void CreateMove( CDOTAInput* rcx, int unk, bool unk1 );
		bool BAsyncSendProto( CProtoBufMsgBase* protobufMsg, IProtoBufSendHandler* handler, google::protobuf::Message* responseMsg, unsigned int respMsgID );
		EGCResults SGCRetrieveMessage( ISteamGameCoordinator* thisptr, uint32* punMsgType, ProtoBufMsgHeader_t* pubDest, uint32 cubDest, uint32* pcubMsgSize );
		bool SendNetMessage( INetChannel* thisptr, NetMessageHandle_t* messageHandle, google::protobuf::Message* msg, NetChannelBufType_t type );
		PVOID PostReceivedNetMessage( INetChannel* rcx,
			CNetworkSerializerPB* rdx,
			google::protobuf::Message* r8,
			NetChannelBufType_t* r9 );
		HRESULT Present( IDXGISwapChain* pSwapchain, UINT SyncInterval, UINT Flags );
		LRESULT WndProc( const HWND hWnd, unsigned int uMsg, uintptr_t wParam, uintptr_t lParam );
		void OnMouseWheeled( CDOTA_Camera* thisptr, int delta );
	}
}

class EntityEventListener : public IEntityListener {

	void OnEntityCreated( C_BaseEntity* ) override;
	void OnEntityDeleted( C_BaseEntity* ) override;

	void OnEntitySpawned( C_BaseEntity* ) override { }
	void OnEntityParentChanged( C_BaseEntity*, C_BaseEntity* parent ) override { };
};

inline EntityEventListener* g_pEntityListener{};