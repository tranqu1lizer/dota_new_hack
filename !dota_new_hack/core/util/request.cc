#include "request.h"

void get_request::init(const char* site)
{
	hIntSession = InternetOpenA("Test", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hIntSession != NULL) 
		hHttpSession = InternetConnectA(hIntSession, site, 80, 0, 0, INTERNET_SERVICE_HTTP, 0, NULL);
	
}
void get_request::de_init()
{
	InternetCloseHandle(hHttpRequest);
	InternetCloseHandle(hHttpSession);
	InternetCloseHandle(hIntSession);
}
void get_request::send(const char* Url,std::string &Buffer)
{
	hHttpRequest = HttpOpenRequestA(hHttpSession, "GET", Url, 0, 0, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE, 0);
	CHAR* szHeaders = "Content-Type: text/html"; CHAR szReq[1024] = "";
	HttpSendRequestA(hHttpRequest, szHeaders, strlen(szHeaders), szReq, strlen(szReq));
	CHAR szBuffer[1025]; DWORD dwRead = 0;
	while (InternetReadFile(hHttpRequest, szBuffer, sizeof(szBuffer) - 1, &dwRead) && dwRead)
	{
		szBuffer[dwRead] = 0;
		dwRead = 0;
	}
	Buffer.append(szBuffer, sizeof(szBuffer));
}