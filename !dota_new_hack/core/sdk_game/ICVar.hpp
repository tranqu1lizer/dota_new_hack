#pragma once

#include "NormalClass.hpp"
#include <span>

#define FCVAR_NONE 0
#define FCVAR_LINKED_CONCOMMAND (1 << 0)
#define FCVAR_DEVELOPMENTONLY (1 << 1)
#define FCVAR_GAMEDLL (1 << 2)
#define FCVAR_CLIENTDLL (1 << 3)
#define FCVAR_HIDDEN (1 << 4)
#define FCVAR_PROTECTED (1 << 5)
#define FCVAR_SPONLY (1 << 6)
#define FCVAR_ARCHIVE (1 << 7)
#define FCVAR_NOTIFY (1 << 8)
#define FCVAR_USERINFO (1 << 9)
#define FCVAR_SOMETHING_THAT_HIDES (1 << 10) // Actual name unavailable
#define FCVAR_UNLOGGED (1 << 11)
#define FCVAR_MISSING1 (1 << 12)
#define FCVAR_REPLICATED (1 << 13)
#define FCVAR_CHEAT (1 << 14)
#define FCVAR_PER_USER (1 << 15)
#define FCVAR_DEMO (1 << 16)
#define FCVAR_DONTRECORD (1 << 17)
#define FCVAR_MISSING2 (1 << 18)
#define FCVAR_RELEASE (1 << 19)
#define FCVAR_MENUBAR_ITEM (1 << 20)
#define FCVAR_MISSING3 (1 << 21)
#define FCVAR_NOT_CONNECTED (1 << 22)
#define FCVAR_VCONSOLE_FUZZY_MATCHING (1 << 23)
#define FCVAR_SERVER_CAN_EXECUTE (1 << 24)
#define FCVAR_MISSING4 (1 << 25)
#define FCVAR_SERVER_CANNOT_QUERY (1 << 26)
#define FCVAR_VCONSOLE_SET_FOCUS (1 << 27)
#define FCVAR_CLIENTCMD_CAN_EXECUTE (1 << 28)
#define FCVAR_EXECUTE_PER_TICK (1 << 29)

struct ConCommand {
public:
	char* m_name; // 0x0
	char* m_description; // 0x8
	std::int32_t m_flags; // 0x10
private:
	std::int32_t unk0; // 0x14
public:
	void* m_member_accessor_ptr; // 0x18
	some_function m_callback; // 0x20
private:
	void* unk1; // 0x28
	void* unk2; // 0x30
};

enum class EConvarType : std::uint8_t
{
	Bool,
	Int16,
	UInt16,
	Int32,
	UInt32,
	Int64,
	UInt64,
	Float32,
	Float64,
	String,
	Color,
	Vector2,
	Vector3,
	Vector4,
	Qangle
};

union ConVarValue_t
{
	bool boolean{};
	std::uint64_t u64;
	std::int64_t i64;
	std::uint32_t u32;
	std::int32_t i32;
	float flt;
	double dbl;
	const char* str;
	std::uint32_t clr_rgba;
	vector2d vec2d;
	vector3d vec3d;
	std::array<float, 4> vec4d;
	QAngle qangle;
};

struct ConVariable
{
	const char* name{};
	void* next_convar_node_like_shit{};
	void* unk1{};
	void* unk2{};
	const char* help{};
	EConvarType type{};
	int unk3{};
	int flags{};
	int unk4{};
	int CALLBACK_INDEX{};
	int unk5{};
	ConVarValue_t m_value;
};

struct CvarNode
{
	ConVariable* var{};
	int index{};
};

struct ConVarID
{
	static inline constexpr auto BAD_ID = 0xFFFFFFFF;
	std::uint64_t impl{};
	void* var_ptr{};

	bool IsGood( ) const noexcept {
		return impl != BAD_ID;
	}

	void invalidate( ) noexcept {
		impl = BAD_ID;
	}
};

using t_CvarCallback = void(*)(const ConVarID& id, int unk1, const ConVarValue_t* val, const ConVarValue_t* old_val);

class ICVar : VClass {
	static auto GetInstanceImpl( )
	{
		static ICVar* inst = static_cast<ICVar*>( util::get_interface( "tier0.dll", "VEngineCvar007" ) );
		return inst;
	}
public:
	static auto& get( )
	{
		return *GetInstanceImpl( );
	}

	std::span<CvarNode> cvars( ) {
		return std::span<CvarNode>{ Member<CvarNode*>( 0x40 ), Member<std::uint16_t>( 0x58 ) };
	}

	std::span<ConCommand> ccommands( ) {
		return std::span<ConCommand>{ Member<ConCommand*>( 0xD8 ), Member<std::uint16_t>( 0xF0 ) };
	}

	t_CvarCallback GetCVarCallback( int index )
	{
		if ( index ) {
			if ( auto table = Member<void*>( 0x80 ); table )
				return *reinterpret_cast<t_CvarCallback*>(reinterpret_cast<std::uintptr_t>(table) + 24 * static_cast<unsigned long long>(index));
		}

		return nullptr;
	}

	ConVariable* find_convar( const std::string_view& name, int& index ) {
		for ( const auto& [cvar_node, idx] : this->cvars( ) )
		{
			if ( cvar_node && name == cvar_node->name ) {
				index = idx;
				return cvar_node;
			}
		}

		return nullptr;
	}

	ConVariable* operator[]( const std::string_view& name ) {
		for ( const auto& [cvar_node, idx] : this->cvars( ) )
		{
			if ( cvar_node && name == cvar_node->name ) {
				return cvar_node;
			}
		}

		return nullptr;
	}
};