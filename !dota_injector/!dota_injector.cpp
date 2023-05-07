#include <windows.h>

__forceinline int strlen_(const char* string)
{
	int cnt = 0;
	if (string)
	{
		for (; *string != 0; ++string) ++cnt;
	}
	return cnt;
}

int main() {
	char m_dll[MAX_PATH];
	GetFullPathNameA("!dota_new_hack.dll", MAX_PATH, m_dll, 0);
	const auto m_process = FindWindowA("SDL_app", "Dota 2");
	unsigned long m_pid;
	GetWindowThreadProcessId(m_process, &m_pid);

	void* m_loadlibrary = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	void* m_handle = OpenProcess(PROCESS_ALL_ACCESS, false, m_pid);
	void* m_write = VirtualAllocEx(m_handle, nullptr, strlen_(m_dll) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if(m_write)
	WriteProcessMemory(m_handle, m_write, m_dll, strlen_(m_dll) + 1, nullptr);

	CreateRemoteThread(m_handle, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(m_loadlibrary), m_write, 0, nullptr);
	CloseHandle(m_handle);
}