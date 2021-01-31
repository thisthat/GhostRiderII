// GhostInjector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <codecvt>
#include <cstdio>
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>


int get_tomb_id() {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (!wcscmp(entry.szExeFile, L"Tomb2.exe"))
            {
                return entry.th32ProcessID;
            }
        }
    }

    CloseHandle(snapshot);
    return -1;
}

int inject(int pid, const std::wstring& dll_path) {
	HANDLE token;
	TOKEN_PRIVILEGES tkp;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
	{
		LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges(token, 0, &tkp, sizeof(tkp), nullptr, nullptr);
	}

	size_t dll_size = (dll_path.length() * 2) + 2;

	HANDLE game_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (!game_handle || game_handle == INVALID_HANDLE_VALUE) {
		std::cout << "Error while opening the process\n";
		return 2;
	}
	auto allocated_mem = VirtualAllocEx(game_handle, nullptr, dll_size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (!allocated_mem) {
		std::cout << "Error while allocating memory\n";
		return 3;
	}

	if (!WriteProcessMemory(game_handle, allocated_mem, dll_path.c_str(), dll_size, 0)) {
		std::cout << "Error while writing on the allocated memory\n";
		return 4;
	}

	HANDLE thread = CreateRemoteThread(game_handle, nullptr, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibrary(L"kernel32.dll"), "LoadLibraryW"), allocated_mem, 0, nullptr);

	if (!thread) {
		std::cout << "Error while creating the listenign library thread\n";
		return 5;
	}

	WaitForSingleObject(thread, INFINITE);

	DWORD ret = 0;

	GetExitCodeThread(thread, &ret);

	CloseHandle(thread);
	CloseHandle(game_handle);
	return ret;
}

int main(int argc, char **argv)
{
	char path[_MAX_PATH + 1];
	GetModuleFileNameA(NULL, path, _MAX_PATH);
	std::string str = std::string(path);
	std::string base_path = str.substr(0, str.find_last_of("\\/") + 1);
	
    const auto dll_path =  base_path + "\\GhostRaiderII.dll";
    int pid = get_tomb_id();
    if (pid > 0) {
		std::wstring l_dll_path = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(dll_path);
        int exit = inject(pid, l_dll_path);
		std::cout << "Injected: " << dll_path << "\nProcess ID:" << pid << "\n";
		return exit;
    }
	return 1;
}
