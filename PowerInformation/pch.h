// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define BOOST_USE_WINAPI_VERSION	0x0601

#define COMPFOLDER 	L"PowerInformation"


#include <iostream>
#include <Windows.h>
#include <powersetting.h>
#include <powrprof.h>
#include <cstdlib>
#include <cstdio>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string>
#include <string_view>
#include <filesystem>
#include <map>
#include <functional>
#include <regex>
#include <chrono>
#include <future>
#include <mutex>
#include <unordered_set>
#include <sstream>
#include <algorithm>

#include <wil/result.h>
#include <wil/resource.h>

//Windows
#include <windows.h>
#include <Shlobj.h>
#include <atlconv.h>
#include <atltrace.h>


//namespaces

namespace	fs = std::filesystem;


#endif //PCH_H
