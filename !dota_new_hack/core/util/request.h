#pragma once

#include <windows.h>
#include <wininet.h>
#pragma comment(lib,"wininet")
#include <string>

class get_request
{
public:
	void init(const char* site);
	void de_init();
	void send(const char* Url, std::string &Buffer);
private:
	HINTERNET hIntSession;
	HINTERNET hHttpSession;
	HINTERNET hHttpRequest;
};
