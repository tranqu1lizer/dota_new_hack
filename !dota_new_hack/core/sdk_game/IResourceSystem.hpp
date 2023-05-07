#pragma once

#include "NormalClass.hpp"

typedef uint64_t ResourceId_t;

template<std::size_t Size>
class ResourceType_t
{
public:
	std::array<char, 8> value{};

	constexpr ResourceType_t( ) = default;
	constexpr ResourceType_t( const char( &Str )[Size] )
	{
		std::ranges::copy( Str, value.begin( ) );
	}
public:
	constexpr const auto& Get( ) const noexcept { return *reinterpret_cast<const std::uint64_t*>(value.data( )); }
};

class IResourceSystem : VClass {
	static auto GetInstanceImpl( )
	{
		static IResourceSystem* inst = nullptr;
		if ( !inst ) inst = static_cast<IResourceSystem*>( util::get_interface( "resourcesystem.dll", "ResourceSystem013" ) );

		return inst;
	}
public:
	static auto& GetInstance( )
	{
		return *GetInstanceImpl( );
	}

	template<ResourceType_t resource_type>
	auto GetResourceByHash( const ResourceId_t id ) const
	{
		const auto ret = CallVFunc<32, uintptr_t>( id, resource_type.Get( ) );
		const auto str_name = reinterpret_cast<const char*>(*reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(ret + 0x8)));
		return str_name;
	}

	/*virtual bool Connect(CreateInterfaceFn factory) = 0;
	virtual void Disconnect() = 0;
	virtual void* QueryInterface(const char* interfaceName) = 0;
	virtual InitReturnVal_t Init() = 0;
	virtual void Shutdown() = 0;
	virtual void PreShutdown() = 0;
	virtual const AppSystemInfo_t* GetDependencies() = 0;
	virtual AppSystemTier_t GetTier() = 0;
	virtual void Reconnect(CreateInterfaceFn factory, const char* pInterfaceName) = 0;
	virtual bool IsSingleton() = 0;
	virtual void GetBuildType() = 0;

	virtual void InstallTypeManager(uintptr_t, uintptr_t, uintptr_t, uintptr_t) = 0; // 11
	virtual uintptr_t InstallNullTypeManager(uintptr_t, uintptr_t) = 0;
	virtual void Update(int, int) = 0;
	virtual void UpdateSimple() = 0;
	virtual bool HasPendingWork() = 0;
	virtual uintptr_t sub_180011130(uintptr_t) = 0;
	virtual uintptr_t RemoveResourceTypeManager(uintptr_t) = 0; // 17
	virtual uintptr_t GetTypeManager(uintptr_t) = 0;
	virtual uintptr_t RegisterFinishedFrameCounter(unsigned long*) = 0;
	virtual uintptr_t UnregisterFinishedFrameCounter(uintptr_t, uintptr_t) = 0;
	virtual void sub_180015E40(void) = 0;
	virtual void sub_180015C80(void) = 0;
	virtual void sub_180015D60(void) = 0;
	virtual void sub_180015E90(void) = 0;
	virtual void sub_180016070(void) = 0;
	virtual void sub_1800148D0(void) = 0;
	virtual void sub_180016020(void) = 0;
	virtual void sub_180016090(void) = 0;
	virtual void sub_180018A70(void) = 0;
	virtual uintptr_t FindOrCreateProceduralResource(const char*, uintptr_t, int) = 0;
	virtual uintptr_t FindOrRegisterResourceByName(const char*, bool) = 0;
	virtual uintptr_t FindResourceById(ResourceId_t, uintptr_t) = 0; // 32*/
	///...
};