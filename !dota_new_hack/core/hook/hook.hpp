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
		inline void* fpFrameStageNotify;
		inline void* fpBAsyncSendProto;
		inline void* fpOnMouseWheeled;
		inline void* fpSGCRetrieveMessage;
		inline void* fpSendNetMessage;
		inline void* fpPostReceivedNetMessage;
		inline void* fpPresent;
		inline void* fpWndProc;
	}

	namespace functions {
		void FrameStageNotify( CSource2Client*, ClientFrameStage_t );
		void CreateMove( CDOTAInput*, int, bool );
		bool BAsyncSendProto( CProtoBufMsgBase*, IProtoBufSendHandler*, google::protobuf::Message*, unsigned int );
		EGCResults SGCRetrieveMessage( ISteamGameCoordinator*, uint32*, ProtoBufMsgHeader_t*, uint32, uint32* );
		bool SendNetMessage( INetChannel*, NetMessageHandle_t*, google::protobuf::Message*, NetChannelBufType_t );
		PVOID PostReceivedNetMessage( INetChannel*, CNetworkSerializerPB*, google::protobuf::Message*, NetChannelBufType_t* );
		HRESULT Present( IDXGISwapChain*, UINT, UINT );
		LRESULT WndProc( const HWND, unsigned int, uintptr_t, uintptr_t );
		void OnMouseWheeled( CDOTA_Camera*, int );
	}
}

class EntityEventListener : public IEntityListener {

	void OnEntityCreated( C_BaseEntity* ) override;
	void OnEntityDeleted( C_BaseEntity* ) override;

	void OnEntitySpawned( C_BaseEntity* ) override { }
	void OnEntityParentChanged( C_BaseEntity*, C_BaseEntity* parent ) override { };
};

inline EntityEventListener* g_pEntityListener{};