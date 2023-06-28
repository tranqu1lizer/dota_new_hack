#pragma once

#include "../lib/protobuf/files/econ_gcmessages.pb.h"
#include "../lib/protobuf/files/dota_clientmessages.pb.h"
#include "../lib/protobuf/files/dota_gcmessages_client.pb.h"
#include "../lib/protobuf/files/clientmessages.pb.h"
#include "../lib/protobuf/files/dota_gcmessages_client_match_management.pb.h"
#include "../lib/protobuf/files/netmessages.pb.h"
#include "../lib/protobuf/files/dota_gcmessages_client_chat.pb.h"
#include "../lib/protobuf/files/dota_usermessages.pb.h"
#include "../lib/protobuf/files/usermessages.pb.h"
#include "../lib/protobuf/files/usercmd.pb.h"
#include "../lib/protobuf/files/dota_usercmd.pb.h"
#include "../lib/protobuf/files/gameevents.pb.h"

struct NetMessageInfo_t
{
	void* pad;
	void* pProtobufBinding;
};

#define _BYTE BYTE
#define _DWORD DWORD
#define _QWORD std::uintptr_t

enum NetChannelBufType_t : int
{
	BUF_DEFAULT = -1,
	BUF_UNRELIABLE = 0,
	BUF_RELIABLE = 1,
	BUF_VOICE = 2,
};

enum NetMessageGroups : int
{
	SYSTEM = 0,
	ENTITIES = 1,
	STRING_COMMAND = 2,
	SIGNON = 3,
	SPAWNGROUPS = 4,
	MOVE = 5,
	VOICE = 6,
	GENERIC = 7,
	STRING_TABLE = 8,
	SOUNDS = 9,

	EVENTS = 12,
	CLIENT_MESSAGES = 13,
	USER_MESSAGES = 14,
	DECALS = 15,
};

enum NetworkSerializationMode_t : int
{
	NET_SERIALIZATION_MODE_0 = 0,
	NET_SERIALIZATION_MODE_DEFAULT = 0,
	NET_SERIALIZATION_MODE_SERVER = 0,
	NET_SERIALIZATION_MODE_1 = 1,
	NET_SERIALIZATION_MODE_CLIENT = 1,
	NET_SERIALIZATION_MODE_COUNT = 2,
};

typedef uint32 AppId_t;
typedef uint64 JobID_t;

enum EMsgFormatType
{
	k_EMsgFormatTypeStruct = 0,
	k_EMsgFormatTypeClientStruct = 1,
	k_EMsgFormatTypeClientStructDeprecated = 2,
	k_EMsgFormatTypeProtocolBuffer = 3
};

class CNetPacket
{
public:
	int m_cRef;					// reference count, deletes self when 0
	uint32_t m_cubData;
	void* m_pubData;
};
class IMsgNetPacket
{
public:

	virtual EMsgFormatType GetEMsgFormatType( ) const = 0;
	virtual CNetPacket* GetCNetPacket( ) const = 0;
	virtual uint8_t* PubData( ) const = 0;
	virtual uint32_t CubData( ) const = 0;
	virtual int GetEMsg( ) const = 0; // EDOTAGCMsg
	virtual JobID_t GetSourceJobID( ) const = 0;
	virtual JobID_t GetTargetJobID( ) const = 0;
	virtual void SetTargetJobID( JobID_t ulJobID ) = 0;
	virtual int64_t GetSteamID( ) const = 0;
	virtual void SetSteamID( int64_t steamID ) = 0;
	virtual AppId_t GetSourceAppID( ) const = 0;
	virtual void SetSourceAppID( AppId_t appId ) = 0;
	virtual bool BHasTargetJobName( ) const = 0;
	virtual const char* GetTargetJobName( ) const = 0;

	inline void GetMsgBody( const uint8_t** msg, uint32_t* msgSize )
	{
		unsigned int messageSize = *(unsigned int*)( this->PubData( ) + 4 );
		messageSize += 8; // min size
		*msg = this->PubData( ) + messageSize;
		*msgSize = this->CubData( ) - messageSize;
	}

	inline int GetMessageID( ) {
		return *reinterpret_cast<int*>( reinterpret_cast<std::uintptr_t>( this ) + 0x78 );
	}
};

class CMsgProtoBufHeader;
class IProtoBufSendHandler
{
public:
	virtual bool BAsyncSend( int EDOTAGCMsg, const uint8_t* pubMsgBytes, uint32 cubSize ) = 0;
};

class CProtoBufMsgBase : public VClass {
private:
	void* unk;
public:
	google::protobuf::Message* header;
	int msgID; // EDOTAGCMsg
	google::protobuf::Message* msg;
};

class CProtobuffBinding // AKA CNetMessagePB
{
public:
	virtual const char* GetName( void ) = 0;
	virtual int GetSize( void ) = 0;
	virtual const char* ToString( google::protobuf::Message* msg, void* t ) = 0;
	virtual const char* GetGroup( void ) = 0;
	virtual ColorRGBA GetGroupColor( void ) = 0;
	virtual NetChannelBufType_t GetBufType( void ) = 0;
	virtual bool ReadFromBuffer( google::protobuf::Message* msg, void* ) = 0; // true if parsing O = 0K
	virtual bool WriteToBuffer( google::protobuf::Message* msg, void* ) = 0; // true if parsing O = 0K bf_write&
	virtual void* AllocateMessage( void ) = 0;
	virtual void DeallocateMessage( void* ) = 0;
	virtual void* AllocateAndCopyConstructNetMessage( void const* otherProtobufMsg ) = 0;
	virtual bool OkToRedispatch( void ) = 0;
	virtual void Copy( void const*, void* ) = 0;
};

class CNetworkSerializerPB // PB must = protobuf
{
public:
	virtual ~CNetworkSerializerPB( ) {};
	virtual const char* GetUnscopedName( void ) = 0;
	virtual void* GetNetMessageInfo( void ) = 0;
	virtual void SetMessageID( unsigned int ) = 0;
	virtual void AddCategoryMask( unsigned int bitflag, bool applyToAnotherMember ) = 0;
	virtual void SwitchMode( int networkValidationMode_t ) = 0;
	virtual void AllocateMessage( void ) = 0;
	virtual void DeallocateMessage( void* unk ) = 0;
	virtual void AllocateAndCopyConstructNetMessage( void const* ) = 0;
	virtual void Serialize(void* /*bf_write&*/, void const*, NetworkSerializationMode_t ) = 0;
	virtual void UnSerialize( void ) = 0;

	const char* unscopedName;
	uint32_t categoryMask;
	int unk;
	CProtobuffBinding* protobufBinding;
	const char* groupName;
	int16_t messageID;
	uint8_t groupID;
	uint8_t defaultBufferType;
	char _pad[28];
};

typedef void* NetMessageHandle_t;

// Find SetMaxRoutablePayloadSize with "Setting max routable payload" (libnetworksystem)
// Or "CNetChan::SendNetMessage" to SendNetMessage()
class INetChannel
{
public:
	virtual const char* GetName( void ) = 0;// get channel name
	virtual const char* GetAddress( void ) = 0;// get channel IP address as string
	virtual float GetTime( void ) = 0;// current net time
	virtual float GetTimeConnected( void ) = 0;// get connection time in seconds
	virtual int GetBufferSize( void ) = 0;// netchannel packet history size
	virtual int GetDataRate( void ) = 0;// send data rate in byte/sec
	virtual bool IsLocalHost( void ) = 0;
	virtual bool IsLoopback( void ) = 0;// true if loopback channel
	virtual bool IsTimingOut( void ) = 0;// true if timing out
	virtual bool IsPlayback( void ) = 0;// true if demo playback
	virtual float GetLatency( int ) = 0;// current latency (RTT), more accurate but jittering
	virtual float GetAvgLatency( int ) = 0;// average packet latency in seconds
	virtual float GetStdDevLatency( int ) = 0;
	virtual void* GetLatencyBreakdown( void* NetChanStat_t, void* NetChanStat_t2, void* NetChanStat_t3, void* NetChanStat_t4 ) = 0;
	virtual float _unk( ) = 0;
	virtual float _unk1( ) = 0;
	virtual float _unk2( ) = 0;
	virtual void* GetTotalData( int ) = 0;
	virtual void* GetTotalPackets( int ) = 0;
	virtual int GetSequenceNr( int ) = 0;
	virtual bool  IsValidPacket( int, int ) = 0;
};

class IPeerToPeerCallbacks;
class IConnectionlessPacketHandler;

// xref "Leaked channel %s remote %s\n" to Shutdown()
// or xref "CNetworkSystem::ShutdownGameServer" to ShutdownGameServer()
class CNetworkSystem : public IAppSystem
{
	static auto GetInstanceImpl( )
	{
		static CNetworkSystem* inst = nullptr;
		if ( !inst ) inst = static_cast<CNetworkSystem*>( util::get_interface( "networksystem.dll", "NetworkSystemVersion001" ) );

		return inst;
	}
public:
	static auto& GetInstance( )
	{
		return *GetInstanceImpl( );
	}
};

#define CASE_STRING( x ) case static_cast<int>( x ) : return #x
#define CASE_STD_STRING( x ) case static_cast<int>( x ) : return std::string(#x)

struct BaseProtobufMessage {
	virtual ~BaseProtobufMessage( ) {}
	virtual std::string GetTypeName( ) { return ""; }
	virtual void* New( void* ) { return nullptr; }
	virtual void Clear( ) {}
	virtual bool IsInitialized( ) { return true; }
	virtual std::string InitializationErrorString( ) { return ""; }
	virtual void CheckTypeAndMergeFrom( void* ) {}
	virtual size_t ByteSizeLong( ) const { return 0; }
	virtual int GetCachedSize( ) const { return 0; }
	virtual void* _InternalParse( void*, void* ) { return nullptr; }
	virtual void OnDemandRegisterArenaDtor( void* ) {}
	virtual uint8_t* _InternalSerialize( uint8_t* target, ::google::protobuf::io::EpsCopyOutputStream* stream ) const {
		return target;
	}
	virtual size_t ByteSizeLongInternal( ) const { return 0; }
	FORCEINLINE std::string SerializeAsString( ) const {
		std::string res;
		this->SerializeToString( res );
		return res;
	}
	FORCEINLINE void SerializeToString( std::string& out ) const {
		auto byte_size = this->ByteSizeLong( );
		out.resize( byte_size );
		this->SerializeToArrayImpl( (uint8_t*)out.data( ), byte_size );
	}
	FORCEINLINE bool SerializeToArray( void* data, int size ) const {
		auto byte_size = this->ByteSizeLong( );
		if ( byte_size > size )
			return false;
		this->SerializeToArrayImpl( (uint8_t*)data, size );
		return true;
	}

private:
	FORCEINLINE uint8_t* SerializeToArrayImpl( uint8_t* target, int size ) const {
		::google::protobuf::io::EpsCopyOutputStream out( target, size, false );
		auto res = this->_InternalSerialize( target, &out );
		GOOGLE_DCHECK( target + size == res );
		return res;
	}
	void* _internal_metadata_ = nullptr;
};

struct GCMsgHdr_t
{
	uint32_t	m_eMsg;					// The message type
	uint64_t	m_ulSteamID;				// User's SteamID
};

struct GCMsgHdrEx_t
{
	uint32_t	m_eMsg;					// The message type
	uint64_t	m_ulSteamID;				// User's SteamID
	uint16_t  m_nHdrVersion;
	JobID_t m_JobIDTarget;
	JobID_t m_JobIDSource;
};