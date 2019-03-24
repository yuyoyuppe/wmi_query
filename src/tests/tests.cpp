#include "pch.h"

#include <log.h>

#include <wmi_classes.h>

int main()
{
  for(const auto monitorID : WmiMonitorID::get_all_objects())
  {
    const auto str = monitorID.to_string();
    log(info, "woah: %s", str.c_str());
  }
  return 0;
}