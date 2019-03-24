#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

struct WinAPIError
{
  WinAPIError(const HRESULT result, const char* file, const int line);
};

#define CHECKED(expr) WinAPIError(expr, __FILE__, __LINE__)