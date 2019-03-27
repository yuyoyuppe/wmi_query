#include "pch.h"

#include <log.h>

#include <wmi_classes.h>

int main()
{
  for(const auto obj : Win32_UserProfile::get_all_objects())
  {
    const auto str = obj.to_string();
    log(info, "object:\n%s\n", str.c_str());
  }
  return 0;
}