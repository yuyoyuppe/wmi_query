#pragma once
#include <string>
#include <vector>
struct WmiMonitorID
{
  bool Active;
  uint8_t WeekOfManufacture;
  std::string SerialNumberID;
  std::string InstanceName;
  std::string ManufacturerName;
  std::string ProductCodeID;
  std::string UserFriendlyName;
  uint16_t YearOfManufacture;

  static std::vector<WmiMonitorID> get_all_objects();
  std::string to_string() const;
};

