#pragma once

struct AppSystemInfo_t
{
	const char* m_pModuleName;
	const char* m_pInterfaceName;
};

typedef void* ( *InstantiateInterfaceFn ) ( );

struct InterfaceReg
{
	some_function m_CreateFn;
	const char* m_pName;
	InterfaceReg* m_pNext;
};

enum InitReturnVal_t
{
	INIT_FAILED = 0,
	INIT_OK,
	INIT_LAST_VAL,
};

enum AppSystemTier_t
{
	APP_SYSTEM_TIER0 = 0,
	APP_SYSTEM_TIER1,
	APP_SYSTEM_TIER2,
	APP_SYSTEM_TIER3,
	APP_SYSTEM_TIER_OTHER,
};

class IAppSystem
{
public:
	virtual bool Connect( void* factory ) = 0;
	virtual void Disconnect() = 0;
	virtual void* QueryInterface(const char* interfaceName) = 0;
	virtual InitReturnVal_t Init() = 0;
	virtual void Shutdown() = 0;
	virtual void PreShutdown() = 0;
	virtual const AppSystemInfo_t* GetDependencies() = 0;
	virtual AppSystemTier_t GetTier() = 0;
	virtual void Reconnect(void* factory, const char* pInterfaceName) = 0;
	virtual bool IsSingleton() = 0;
	virtual void GetBuildType() = 0;
};

class CLocalize : public VClass{
	static auto GetInstanceImpl( )
	{
		static CLocalize* inst = nullptr;
		if ( !inst ) inst = static_cast<CLocalize*>( util::get_interface( "localize.dll", "Localize_001" ) );

		return inst;
	}
public:
	static auto& GetInstance( )
	{
		return *GetInstanceImpl( );
	}

	auto FindString( const std::string_view& str ) {
		return CallVFunc<16, const char*>( str.data() );
	}

	auto FindStringSafely( const std::string_view& str ) {
		return CallVFunc<17, const char*>( str.data() );
	}

	const wchar_t* ConvertANSIToUnicode( const char* str ) {
		return CallVFunc<18, const wchar_t*>( str );
	}
};