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

int GetPath(std::wstring dirname, int accessFlags)
{
	printf("\tGetPath: Testing access flags 0x%08X with %ws\r\n", accessFlags, dirname.c_str());

	auto hnd = CreateFileW(dirname.c_str(), accessFlags,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
		OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);

	if (hnd == INVALID_HANDLE_VALUE) {
		errno = GetLastError();
		printf("\tGetPath: CreateFileW got error %X\r\n", errno);
		return -1;
	}

	wchar_t wdirname[MAX_PATH] = { 0 };
	if (!GetFinalPathNameByHandleW(hnd, wdirname, sizeof wdirname / sizeof(wchar_t), 0))
	{
		errno = GetLastError();
		printf("\tGetPath: GetFinalPathNameByHandleW got error %X\r\n", errno);
		CloseHandle(hnd);
		return -1;
	}

	printf("\tGetPath: GetFinalPathNameByHandleW returned: %ws\r\n", wdirname);

	CloseHandle(hnd);
	return 0;
}

int GetPathExclusive(std::wstring dirname, int sharingFlags)
{
	printf("GetPathExclusive: Testing sharing flags 0x%08X\r\n", sharingFlags);

	auto hnd = CreateFileW(dirname.c_str(), GENERIC_READ | GENERIC_WRITE,
		sharingFlags, nullptr,
		OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);

	if (hnd == INVALID_HANDLE_VALUE) {
		errno = GetLastError();
		printf("CreateFileW got error %X\r\n", errno);
		return -1;
	}

	GetPath(dirname, 0);
	GetPath(dirname, GENERIC_READ);

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

	printf("\r\nSecond test: open the file first without sharing, then try to get the path\r\n");
	GetPathExclusive(dir, 0);

	printf("\r\nThird test: open the file first with FILE_SHARE_WRITE, then try to get the path\r\n");
	GetPathExclusive(dir, FILE_SHARE_WRITE);

	printf("\r\nFourth test: open the file first with FILE_SHARE_READ, then try to get the path\r\n");
	GetPathExclusive(dir, FILE_SHARE_READ);
}