#pragma once

#include "../global.hpp"

class DataTeamPlayer_t{
public:
	int GetNetWorth() {
		if ( !this ) return -1;
		return *reinterpret_cast<int*>(this + 0x7c);
	}
	
	int GetDenies() {
		if ( !this ) return -1;
		return *reinterpret_cast<int*>(this + 0x80);
	}
	
	int GetLastHits() {
		if ( !this ) return -1;
		return *reinterpret_cast<int*>(this + 0x84);
	}
};