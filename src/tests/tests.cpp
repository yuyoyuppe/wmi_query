#include "pch.h"

#include <log.h>

#include <wmi_classes.h>

int main()
{
  const auto objects = wmi::WmiMonitorID::get_all_objects();
  for(const auto& o : objects)
    log(info, "got: %s", o.to_string().c_str());
  
  return 0;
}
