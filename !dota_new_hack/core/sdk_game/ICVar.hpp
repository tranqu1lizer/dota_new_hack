#pragma once

#include "NormalClass.hpp"
#include <span>

enum class EConvarType : std::uint8_t
{
	BOOL = 0,
	INT32,
	UINT32,
	INT64,
	UINT64,
	FLOAT,
	DOUBLE,
	STRING,
	COLOR_RGBA,
	UNK_SOME_TWO_FLOATS,
	UNK_SOME_THREE_FLOATS,
	UNK_SOME_FOUR_FLOATS,
	UNK_SOME_THREE_FLOATS_AGAIN,
};

union ConVarValue
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
	std::array<float, 2> two_floats;
	std::array<float, 3> three_floats;
	std::array<float, 4> four_floats;
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
	ConVarValue value{};
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

	bool IsGood( ) const noexcept
	{
		return impl != BAD_ID;
	}

	void Invalidate( ) noexcept
	{
		impl = BAD_ID;
	}
};

using t_CvarCallback = void(*)(const ConVarID& id, int unk1, const ConVarValue* val, const ConVarValue* old_val);

class ICVar : NormalClass {
	static auto GetInstanceImpl( )
	{
		static ICVar* inst = nullptr;
		if ( !inst ) inst = static_cast<ICVar*>( util::get_interface( "tier0.dll", "VEngineCvar007" ) );
		
		return inst;
	}
public:
	static auto& GetInstance( )
	{
		return *GetInstanceImpl( );
	}

	std::span<const CvarNode> ICVar::GetCvarList( )
	{
		return std::span<const CvarNode>{ Member<const CvarNode*>( 0x40 ), Member<std::uint16_t>( 0x58 ) };
	}

	t_CvarCallback GetCVarCallback( int index )
	{
		if ( index )
		{
			auto table = Member<void*>( 0x80 );
			if ( table )
				return *reinterpret_cast<t_CvarCallback*>(reinterpret_cast<std::uintptr_t>(table) + 24 * static_cast<unsigned long long>(index));
		}
		return nullptr;
	}

	ConVariable* FindConVar( const std::string_view& name, int& index ) {
		for ( const auto& [cvar_node, idx] : this->GetCvarList( ) )
		{
			if ( cvar_node && name == cvar_node->name ) {
				index = idx;
				return cvar_node;
			}
		}
		return nullptr;
	}
};