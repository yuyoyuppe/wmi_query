#include "pch.h"

#include <core_log_constants.h>

const char* to_string(const log_verbosity lvl)
{
  switch(lvl)
  {
  case log_verbosity::error:
    return "error";
  case log_verbosity::warning:
    return "warning";
  case log_verbosity::info:
    return "info";
  case log_verbosity::verbose:
    return "verbose";
  default:
    return "unknown";
  }
}

void log(const log_verbosity lvl, const char * fmt, ...)
{
  if(lvl > konst::log_verbosity_level)
    return;

  constexpr size_t buf_size = 4096;
  char buffer[buf_size];
  const size_t nUnavailableChars = sprintf_s<buf_size>(buffer, "[%s] ", to_string(lvl));
  
  va_list argptr;
  va_start(argptr, fmt);
  vsnprintf_s(buffer + nUnavailableChars, buf_size - nUnavailableChars, buf_size - nUnavailableChars - 2, fmt, argptr);
  va_end(argptr);
  strcat_s<buf_size>(buffer, "\n");

  OutputDebugStringA(buffer);
  printf(buffer);
}
