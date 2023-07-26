#pragma once

#include "../definitions.h"
#include "INetChan.h"

struct NetworkCallback_t
{
private:
	std::uintptr_t unk0;
public:
	char* m_name;
private:
	std::uintptr_t unk1;
public:
	void( *m_func_ptr )( void* );
	char* m_class_name;
private:
	std::uintptr_t unk2[ 2 ];
};

class CNetworkMessages {
	PDEFINE_INTERFACE( CNetworkMessages, "networksystem.dll", "NetworkMessagesVersion001" );

	PAD( 0x4B8 );
	std::array<NetworkCallback_t, 32 * 8>* m_net_callbacks; // 0x4C0

	virtual void RegisterNetworkCategory( unsigned int, const char* ) = 0;
	virtual void AssociateNetMessageWithChannelCategoryAbstract( NetMessageHandle_t*, unsigned int, bool ) = 0;
	virtual NetMessageHandle_t* FindOrCreateNetMessage( int, void const* IProtobufBinding, unsigned int, void* INetworkSerializerBindingBuildFilter, bool, bool ) = 0;
	virtual bool SerializeAbstract( void ) = 0;
	virtual void UnserializeAbstract( void ) = 0;
	virtual void UnserializeAbstract( void*, void* ) = 0;
	virtual void* AllocateUnserializedMessage( NetMessageHandle_t* ) = 0;
	virtual void AllocateAndCopyConstructNetMessageAbstract( NetMessageHandle_t*, void const* ) = 0;
	virtual void DeallocateUnserializedMessage( NetMessageHandle_t*, void* ) = 0;
	virtual void RegisterNetworkFieldSerializer( ) = 0;
	virtual void RegisterNetworkArrayFieldSerializer( ) = 0;
	virtual void* GetNetMessageInfo( NetMessageHandle_t* ) = 0;
	virtual void vfunc12( void ) = 0;
	virtual NetMessageHandle_t* FindNetworkMessage( const char* ) = 0; // 13

	some_function find_network_callback( const char* callback_name ) {
		if ( !m_net_callbacks ) return nullptr;

		for ( auto& callback : *m_net_callbacks ) {
			if ( callback.m_name && callback.m_func_ptr && !util::fast_strcmp( callback.m_name, callback_name ) ) {
				return callback.m_func_ptr;
			}
		}

		return nullptr;
	}
};