#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <pugixml.hpp>

struct APIError
{
  APIError(const HRESULT result, const char* file, const int line);
  APIError(const pugi::xml_parse_result result, const char* file, const int line);
};

#define CHECKED(expr) APIError(expr, __FILE__, __LINE__)