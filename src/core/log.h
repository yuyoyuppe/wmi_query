#pragma once
#include <array>

enum log_verbosity : uint8_t
{
  error,
  warning,
  info,
  verbose
};

void log(const log_verbosity lvl, const char * fmt, ...);