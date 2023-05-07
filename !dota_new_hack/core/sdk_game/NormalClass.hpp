#pragma once

class NormalClass
{
public:
	template<class T>
	auto& Member( std::ptrdiff_t offset ) noexcept
	{
		return *reinterpret_cast<T*>( reinterpret_cast<std::uintptr_t>( this ) + offset );
	}

	template<class T>
	const auto& Member( std::ptrdiff_t offset ) const noexcept
	{
		return *reinterpret_cast<const T*>( reinterpret_cast<std::uintptr_t>( this ) + offset );
	}

	template<typename T>
	T* MemberNotPtr( std::ptrdiff_t offset ) {
		return reinterpret_cast<T*>( reinterpret_cast<std::uintptr_t>( this ) + offset );
	}
};

class VFTable {
	void* VTablePtr{ nullptr };
public:
	VFTable( ) = default;
	explicit VFTable( void* vmt ) noexcept
		:
		VTablePtr{ vmt }
	{}

	VFTable( const VFTable& ) = default;
	VFTable& operator=( const VFTable& ) = default;

	VFTable( VFTable&& other ) noexcept
		:
		VTablePtr{ other.VTablePtr }
	{
		other.VTablePtr = nullptr;
	}
	VFTable& operator=( VFTable&& other ) noexcept
	{
		VTablePtr = other.VTablePtr;
		other.VTablePtr = nullptr;
		return *this;
	}
	~VFTable( ) = default;

public:
	bool IsValid( ) const noexcept
	{
		return VTablePtr != nullptr;
	}

	void*& GetFunctionAt( std::size_t index ) noexcept
	{
		return *( static_cast<void**>( VTablePtr ) + index );
	}

	const void*& GetFunctionAt( std::size_t index ) const noexcept
	{
		return *( static_cast<const void**>( VTablePtr ) + index );
	}

	operator void* ( ) noexcept
	{
		return VTablePtr;
	}

	template<typename ReturnType = decltype( VTablePtr )>
	ReturnType GetPtr( ) const noexcept
	{
		return SmartCast<ReturnType>( VTablePtr );
	}
};

class VClass : public NormalClass
{
	VFTable VirtualMethodsTable{};
public:
	VClass( ) = default;

	explicit VClass( const VFTable& vftable ) noexcept : VirtualMethodsTable{ vftable } {}

	template<std::size_t FunctionIndex, typename ReturnType = std::uintptr_t, typename ... ArgTypes>
	ReturnType CallVFunc( ArgTypes... Args ) const
	{
		const auto func = VirtualMethodsTable.GetFunctionAt( FunctionIndex );
		if ( !func )
			throw std::runtime_error{ "CallVFunc -> Func at index is nullptr!" };
		return ( static_cast<ReturnType( __thiscall* )( const VClass*, ArgTypes... )>( func ) )( this, Args... );
	}

	template <class T>
	inline T GetVF( int Index )
	{
		return ( *reinterpret_cast<T**>( this ) )[Index];
	}

	const VFTable& GetVMT( ) const noexcept
	{
		return VirtualMethodsTable;
	}
};

#define GETTER(type, name, offset) type name() {\
if ( util::does_exists( reinterpret_cast<std::uintptr_t*>( this ) ) )\
    return Member<type>( offset );\
throw std::runtime_error{ "getter err" }; }