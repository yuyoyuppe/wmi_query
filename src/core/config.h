#pragma once

#include <core_common_constants.h>

#include <type_traits>
#include <string>

using string_t = std::conditional_t<konst::use_utf8_strings, std::/*u8*/string, std::wstring>;
