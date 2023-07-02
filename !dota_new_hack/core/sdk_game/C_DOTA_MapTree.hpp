#pragma once

#include "C_BaseModelEntity.hpp"

class C_DOTA_MapTree : public C_BaseModelEntity {
public:
	void set_model( const std::string_view& model_name ) {
		const auto C_DOTA_MapTree__Spawn = util::vmt( (std::uintptr_t)this, 7 );
		some_function C_BaseModelEntity__SetModel = util::get_absolute_address( C_DOTA_MapTree__Spawn + 0x1c0, 1, 5 );
		if ( !C_BaseModelEntity__SetModel.valid( ) ) return;

		C_BaseModelEntity__SetModel( this, model_name.data( ) );
	}
};