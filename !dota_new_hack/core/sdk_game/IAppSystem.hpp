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