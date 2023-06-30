#pragma once

#include "../util/util.hpp"

class CDOTAInput;

some_function aGetInput;

class CDOTAInput {
	char pad0[ 0x3D24 ];
public:
	static CDOTAInput* get( ) {
		if ( !aGetInput.ptr ) return nullptr;
		return (CDOTAInput*)aGetInput( );
	}

	int m_sequence_number;
};