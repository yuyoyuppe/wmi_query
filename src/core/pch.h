#pragma once

#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <locale>
#include <codecvt>
#include <variant>
#include <chrono>
#include <optional>
#include <string_view>

#include <stdarg.h>

#define _WIN32_DCOM
#include <comdef.h>
#include <Wbemidl.h>

#include "log.h"
#include "platform_error.h"

constexpr bool eq(const char * a, const char* b)
{
  return std::string_view{ a } == b;
}
