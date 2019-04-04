#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <array>
#include <wmi_api.h>

namespace wmi{

struct WmiMonitorID
{
  bool Active;
  uint8_t WeekOfManufacture;
  std::string InstanceName;
  uint16_t YearOfManufacture;

  std::array<uint16_t, 16> ManufacturerName;
  std::array<uint16_t, 16> ProductCodeID;
  std::array<uint16_t, 16> SerialNumberID;
  std::vector<uint16_t> UserFriendlyName;


  static void deserialize(void * src, WmiMonitorID& destination);
  static std::vector<WmiMonitorID> get_all_objects();
  std::string to_string() const;
};

} //namespace wmi
