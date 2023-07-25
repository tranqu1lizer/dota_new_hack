#pragma once

#define PPCAT_NX(a,b) a##b
#define PPCAT(a,b) PPCAT_NX(a,b)
#define PAD(sz)\
private:\
char PPCAT( _pad, __COUNTER__ )[ sz ];\
public:
#define PPAD(sz) char PPCAT(_pad, __COUNTER__)[sz]; // og

#define CASE_STRING( x ) case static_cast<int>( x ) : return #x // lwss

#define PDEFINE_INTERFACE(T, dll, inter) public:\
__forceinline static T* get( ){ static auto inst = (T*)util::get_interface( dll, inter );return inst;};

#define DEFINE_INTERFACE(T, dll, inter) private:\
static auto GetInstanceImpl( ) {static auto inst = (T*)util::get_interface( dll, inter ); return inst;}\
public: \
static auto& get( ) {return *GetInstanceImpl( );}

#define FIND_FN( dll_name, fn, sig, fn_name, relative_call )\
fn = (decltype(fn))util::find_pattern( (util::fast_strcmp(dll_name, "client.dll")==0) ? global::client : (HMODULE)util::get_module_base_ansi( dll_name ), sig, fn_name ); \
if (relative_call)\
	fn = (decltype(fn))util::get_absolute_address((uintptr_t)fn, 1,5); \
if(!fn) return MessageBoxA( 0, fn_name, 0, MB_ICONERROR );

#define DETOUR_PATTERN( dll_name, sig, sig_name, hk_name, fm, relative_call ) \
hook::install_hook(relative_call ? util::get_absolute_address(util::find_pattern( dll_name, sig, sig_name, fm),1,5) : util::find_pattern( dll_name, sig, sig_name, fm), &hook::functions::##hk_name, &hook::original::fp##hk_name, #hk_name )

#define DETOUR_VF( ptr, index, hk_name, is_vmt ) \
hook::install_hook(is_vmt ? util::vmt1((uintptr_t)ptr, index) : util::vmt((uintptr_t)ptr, index), &hook::functions::##hk_name, &hook::original::fp##hk_name, #hk_name )

#define GAB( var, offset, sz ) util::get_absolute_address(var, offset,sz)

#define CHECK_VAR_RET( var ) \
if(!var) {\
spdlog::critical( "{} is nullptr", #var );\
return 0;\
}