#include "pch.h"

#include "log.h"
#include <core_common_constants.h>

WinAPIError::WinAPIError(const HRESULT result, const char* file, const int line)
{
  if(FAILED(result))
  {
    const DWORD error_code = HRESULT_CODE(result);
    char* error_msg;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      nullptr,
      error_code,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      reinterpret_cast<LPSTR>(&error_msg),
      0,
      nullptr);
    log(error, "winapi call at %s:%d failed: %s", file, line, error_msg);
    LocalFree(error_msg);
    if constexpr(konst::use_exceptions)
      throw result;
  }
}
