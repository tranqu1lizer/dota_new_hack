#pragma once

#include <cstdint>

class ISteamUser;
class ISteamGameServer;
class ISteamFriends;
class ISteamUtils;
class ISteamMatchmaking;
class ISteamContentServer;
class ISteamMatchmakingServers;
class ISteamUserStats;
class ISteamApps;
class ISteamNetworking;
class ISteamRemoteStorage;
class ISteamScreenshots;
class ISteamMusic;
class ISteamMusicRemote;
class ISteamGameServerStats;
class ISteamHTTP;
class ISteamUnifiedMessages;
class ISteamController;
class ISteamUGC;
class ISteamAppList;
class ISteamHTMLSurface;
class ISteamInventory;
class ISteamVideo;

typedef uint64_t SteamAPICallHandle;
typedef int HSteamPipe;
typedef int HSteamUser;

typedef void( __cdecl* SteamAPIWarningMessageHook_t )(int, const char*);
typedef void(*SteamAPI_PostAPIResultInProcess_t)(SteamAPICallHandle callHandle, void*, uint32_t unCallbackSize, int iCallbackNum);
typedef uint32_t( *SteamAPI_CheckCallbackRegistered_t )(int iCallbackNum);

enum EGCResults
{
	k_EGCResultOK = 0,
	k_EGCResultNoMessage = 1,
	k_EGCResultBufferTooSmall = 2,
	k_EGCResultNotLoggedOn = 3,
	k_EGCResultInvalidMessage = 4,
};

enum EAccountType
{
	k_EAccountTypeInvalid = 0,
	k_EAccountTypeIndividual = 1,
	k_EAccountTypeMultiseat = 2,
	k_EAccountTypeGameServer = 3,
	k_EAccountTypeAnonGameServer = 4,
	k_EAccountTypePending = 5,
	k_EAccountTypeContentServer = 6,
	k_EAccountTypeClan = 7,
	k_EAccountTypeChat = 8,
	k_EAccountTypeConsoleUser = 9,
	k_EAccountTypeAnonUser = 10,

	// Max of 16 items in this field
	k_EAccountTypeMax
};

typedef uint32_t MsgType_t;
const uint32_t k_EMsgProtoBufFlag = 0x80000000;

struct ProtoBufMsgHeader_t
{
	int			m_EMsgFlagged;
	uint32_t			m_cubProtoBufExtHdr;

	ProtoBufMsgHeader_t( ) : m_EMsgFlagged( 0 ), m_cubProtoBufExtHdr( 0 ) {}
	ProtoBufMsgHeader_t( MsgType_t eMsg, uint32_t cubProtoBufExtHdr ) : m_EMsgFlagged( eMsg | k_EMsgProtoBufFlag ), m_cubProtoBufExtHdr( cubProtoBufExtHdr ) {}
	MsgType_t GetEMsg( ) const { return (MsgType_t)(m_EMsgFlagged & (~k_EMsgProtoBufFlag)); }
};

class ISteamGameCoordinator;

class ISteamClient
{
	static auto GetInstanceImpl( )
	{
		static ISteamClient* inst = nullptr;
		if ( !inst ) inst = static_cast<ISteamClient*>( util::get_interface( "steamclient64.dll", "SteamClient017" ) );

		return inst;
	}
public:
	static auto& GetInstance( )
	{
		return *GetInstanceImpl( );
	}

	virtual HSteamPipe CreateSteamPipe( ) = 0;
	virtual bool BReleaseSteamPipe( HSteamPipe hSteamPipe ) = 0;
	virtual HSteamUser ConnectToGlobalUser( HSteamPipe hSteamPipe ) = 0;
	virtual HSteamUser CreateLocalUser( HSteamPipe* phSteamPipe, EAccountType eAccountType ) = 0;
	virtual void ReleaseUser( HSteamPipe hSteamPipe, HSteamUser hUser ) = 0;
	virtual ISteamUser* GetISteamUser( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual ISteamGameServer* GetISteamGameServer( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual void SetLocalIPBinding( uint32_t unIP, uint16_t usPort ) = 0;
	virtual ISteamFriends* GetISteamFriends( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual ISteamUtils* GetISteamUtils( HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual ISteamMatchmaking* GetISteamMatchmaking( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual ISteamMatchmakingServers* GetISteamMatchmakingServers( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual ISteamGameCoordinator* GetISteamGenericInterface( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual ISteamUserStats* GetISteamUserStats( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual ISteamGameServerStats* GetISteamGameServerStats( HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual ISteamApps* GetISteamApps( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual ISteamNetworking* GetISteamNetworking( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual ISteamRemoteStorage* GetISteamRemoteStorage( HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual ISteamScreenshots* GetISteamScreenshots( HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual void RunFrame( ) = 0;
	virtual uint32_t GetIPCCallCount( ) = 0;
	virtual void SetWarningMessageHook( SteamAPIWarningMessageHook_t pFunction ) = 0;
	virtual bool BShutdownIfAllPipesClosed( ) = 0;
	virtual ISteamHTTP* GetISteamHTTP( HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual ISteamUnifiedMessages* GetISteamUnifiedMessages( HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual ISteamController* GetISteamController( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual ISteamUGC* GetISteamUGC( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual ISteamAppList* GetISteamAppList( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual ISteamMusic* GetISteamMusic( HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual ISteamMusicRemote* GetISteamMusicRemote( HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual ISteamHTMLSurface* GetISteamHTMLSurface( HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual void Set_SteamAPI_CPostAPIResultInProcess( SteamAPI_PostAPIResultInProcess_t func ) = 0;
	virtual void Remove_SteamAPI_CPostAPIResultInProcess( SteamAPI_PostAPIResultInProcess_t func ) = 0;
	virtual void Set_SteamAPI_CCheckCallbackRegisteredInProcess( SteamAPI_CheckCallbackRegistered_t func ) = 0;
	virtual ISteamInventory* GetISteamInventory( HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
	virtual ISteamVideo* GetISteamVideo( HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
};

class ISteamGameCoordinator
{
public:
	virtual EGCResults SendMessage_( uint32_t unMsgType, const void* pubData, uint32_t cubData ) = 0;
	virtual bool IsMessageAvailable( uint32_t* pcubMsgSize ) = 0;
	virtual EGCResults RetrieveMessage( uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize ) = 0;
};