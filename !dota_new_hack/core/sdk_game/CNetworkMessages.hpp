#pragma once

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
	std::uintptr_t unk2[2];
};

struct CNetworkMessages
{
	char pad_0000[1216];
public:
	static auto get( )
	{
		static CNetworkMessages* inst = nullptr;
		if ( !inst ) inst = static_cast<CNetworkMessages*>( util::get_interface( "networksystem.dll", "NetworkMessagesVersion001" ) );

		return inst;
	}

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
	virtual NetMessageHandle_t* FindNetworkMessage( const char* ) = 0; // 13s

	auto network_callbacks( ) {
		return **( std::array<NetworkCallback_t, 256>** )( (uintptr_t)this + 0x4C0 );
	}

	some_function find_network_callback( const char* callback_name ) {
		for ( auto& callback : network_callbacks( ) ) {
			if ( callback.m_name && callback.m_func_ptr && !util::fast_strcmp(callback.m_name, (char*)callback_name ) ) {
				return callback.m_func_ptr;
			}
		}
		
		return nullptr;
	}
};