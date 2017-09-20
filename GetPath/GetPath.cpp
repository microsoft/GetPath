#include "stdafx.h"
#include <Windows.h>
#include <string>

// Careful calling this with string as it *will* lose embedded null
// use stringTowstring instead
std::wstring LPCSTRToWstring(LPCSTR src)
{
	if (!src) return L"";
	std::string ansi = src;
	return std::wstring(ansi.begin(), ansi.end());
}

int GetPath(std::wstring dirname, int flag)
{
	printf("\tTesting flag 0x%08X with %ws\r\n", flag, dirname.c_str());

	auto hnd = CreateFileW(dirname.c_str(), flag,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
		OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);

	if (hnd == INVALID_HANDLE_VALUE) {
		errno = GetLastError();
		printf("\tCreateFileW got error %X\r\n", errno);
		return -1;
	}

	wchar_t wdirname[MAX_PATH] = { 0 };
	if (!GetFinalPathNameByHandleW(hnd, wdirname, sizeof wdirname / sizeof(wchar_t), 0))
	{
		errno = GetLastError();
		printf("\tGetFinalPathNameByHandleW got error %X\r\n", errno);
		CloseHandle(hnd);
		return -1;
	}

	printf("\tGetFinalPathNameByHandleW returned: %ws\r\n", wdirname);

	CloseHandle(hnd);
	return 0;
}

void main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("Please pass path to file to check\r\n");
		return;
	}

	auto dir = LPCSTRToWstring(argv[1]);
	printf("GetPath testing GetFinalPathNameByHandleW calls against %ws\r\n", dir.c_str());
	printf("\r\nFirst test: don't open file first, just try to get the path\r\n");
	GetPath(dir, 0);
	GetPath(dir, GENERIC_READ);

	printf("\r\nSecond test: open the file without sharing, then try to get the path\r\n");
	auto hnd = CreateFileW(dir.c_str(), GENERIC_READ,
		0, nullptr,
		OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);

	if (hnd == INVALID_HANDLE_VALUE) {
		errno = GetLastError();
		printf("CreateFileW got error %X\r\n", errno);
	}
	else
	{
		GetPath(dir, 0);
		GetPath(dir, GENERIC_READ);
	}

	CloseHandle(hnd);
}

