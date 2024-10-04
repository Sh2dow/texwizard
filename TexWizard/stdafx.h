// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <stdio.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <future>
#include <d3d9.h>
#include "../Modules/inireader/IniReader.h"
#include "../Modules/Detours/src/detours.h"
#include "../Modules/d3d9-wrapper/source/dxsdk/d3dx9tex.h"
#include "../Modules/injector/include/injector/injector.hpp"
// TODO: reference additional headers your program requires here
