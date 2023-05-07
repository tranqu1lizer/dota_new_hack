#pragma once

#include "../util/util.hpp"

class CGameUIService {
	static auto GetInstanceImpl( )
	{
		static CGameUIService* inst = nullptr;
		if ( !inst ) inst = static_cast<CGameUIService*>( util::get_interface( "engine2.dll", "GameUIService_001" ) );

		return inst;
	}
public:
	static auto& GetInstance( )
	{
		return *GetInstanceImpl( );
	}

	virtual char Connect(__int64, __int64) = 0;
	virtual __int64 Disconnect() = 0;
	virtual __int64	QueryInterface() = 0;
	virtual __int64	Init() = 0;
	virtual	__int64  Shutdown_maybe(__int64) = 0;
	virtual void empty() = 0;
	virtual __int64	QueryInterface2() = 0;
	virtual __int64	CTier4AppSystem__GetTier() = 0;
	virtual __int64 CTier2AppSystem__Reconnect(__int64, __int64, __int64) = 0;
	virtual bool	CBaseAppSystem__IsSingleton() = 0;
	virtual __int64	CBaseAppSystem__GetBuildType() = 0;
	virtual char** GetServiceDependencies() = 0;
	virtual __int64	CBaseEngineService__GetName(__int64) = 0;
	virtual __int64	sub_18015FB50() = 0;
	virtual __int64	OnLoopActivate(__int64, __int64) = 0;
	virtual	__int64 OnLoopDeactivate_maybe(__int64) = 0;
	virtual __int64	CBaseEngineService__IsActive(__int64) = 0;
	virtual void CBaseEngineService__SetActive(__int64, bool active) = 0;
	virtual void CBaseEngineService__SetName(void*, __int64) = 0;
	virtual void unk1(void*) = 0;
	virtual __int64	CBaseEngineService__GetServiceIndex(__int64) = 0;
	virtual void CBaseEngineService__SetServiceIndex(__int64, __int16) = 0;
	virtual __int64	IsConsoleEnabled_maybe(__int64, bool) = 0;
	virtual __int64	sub_1801641E0() = 0;
	virtual __int64	sub_1801641F0() = 0;
	virtual char IsConsoleEnabledByCommandLine() = 0;
	virtual __int64	sub_180164250(__int64, char) = 0;
	virtual bool IsConsoleVisible() = 0;
	virtual __int64	ToggleConsoleVisibility(__int64*) = 0;
	virtual __int64	ShowConsole(bool show) = 0; // 30
	virtual __int64	sub_180164840(__int64, unsigned __int8) = 0;
	virtual double GetLastMouseMoveTime(__int64) = 0;
	virtual __int64	GetInputContext(__int64, int) = 0;
	virtual bool IsUsingVGuiConsole_maybe(__int64) = 0;
	virtual __int64	ClearConsole() = 0;
	virtual void* sub_1801632D0(void*) = 0;
	virtual	void* sub_180164AC0(void*) = 0;
	virtual void* sub_180163310(void*) = 0;
	virtual void* sub_180164CE0(void*) = 0;
	virtual void* sub_180164F00(void*) = 0;
	virtual void* sub_180165260(void*) = 0;
	virtual void* sub_1801651C0(void*) = 0;
	virtual bool HandleInputEvent(__int64, int, unsigned long*) = 0;
};