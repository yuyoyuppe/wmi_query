#include "pch.h"
#include "wmi_classes.h"
std::vector<WmiMonitorID> WmiMonitorID::get_all_objects()
{
  std::vector<WmiMonitorID> result;
  WMIProvider::get().query("select * from WmiMonitorID", [&](IWbemClassObject* o) {
    VARIANT v; CIMTYPE type; WmiMonitorID cpp_obj;
    o->Get(L"Active", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.Active);
    o->Get(L"WeekOfManufacture", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.WeekOfManufacture);
    o->Get(L"SerialNumberID", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.SerialNumberID);
    o->Get(L"InstanceName", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.InstanceName);
    o->Get(L"ManufacturerName", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.ManufacturerName);
    o->Get(L"ProductCodeID", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.ProductCodeID);
    o->Get(L"UserFriendlyName", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.UserFriendlyName);
    o->Get(L"YearOfManufacture", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.YearOfManufacture);
    result.emplace_back(std::move(cpp_obj)); VariantClear(&v);
  });
  return result;
}

std::string WmiMonitorID::to_string() const
{
  std::ostringstream oss;
  oss << "Active: " << std::to_string(Active) << std::endl;
  oss << "WeekOfManufacture: " << std::to_string(WeekOfManufacture) << std::endl;
  oss << "SerialNumberID: " << (SerialNumberID) << std::endl;
  oss << "InstanceName: " << (InstanceName) << std::endl;
  oss << "ManufacturerName: " << (ManufacturerName) << std::endl;
  oss << "ProductCodeID: " << (ProductCodeID) << std::endl;
  oss << "UserFriendlyName: " << (UserFriendlyName) << std::endl;
  oss << "YearOfManufacture: " << std::to_string(YearOfManufacture) << std::endl;
  return oss.str();
}
