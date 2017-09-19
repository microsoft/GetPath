# GetPath
Command line test harness for GetFinalPathNameByHandleW

Pass in a file or directory name to test running GetFinalPathNameByHandleW.

In order to run GetFinalPathNameByHandleW, we first need to open the file. We do this using CreateFileW, like so:
```C+
auto hnd = CreateFileW(dirname.c_str(), flag,
   FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
   OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
```

There is some debate about the proper flag to pass. Is GENERIC_READ (0x80000000) required, or can you get by with 0? This code will allow you to test this, since it will try it both ways:
```dos
C:\src\GetPath\Debug>pushd \\machine\c$\tools

Z:\Tools>C:\src\getpath\Debug\GetPath.exe Z:\Tools
Testing flag 0x00000000 with Z:\Tools
GetFinalPathNameByHandleW returned: \\?\C:\Tools
Testing flag 0x80000000 with Z:\Tools
GetFinalPathNameByHandleW returned: \\?\C:\Tools
```
Clearly for this path, either 0 or GENERIC_READ worked. This may not be true for all network devices.
