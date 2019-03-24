#pragma once
#include "config.h"

#include <functional>

struct IWbemLocator;
struct IWbemServices;
struct IWbemClassObject;

string_t wstring_to_string_t(const std::wstring & ws);

class WMIProvider
{
  IWbemLocator *  _locator = nullptr;
  IWbemServices * _services = nullptr;
  WMIProvider();
public:
  ~WMIProvider();
  static WMIProvider& get();
  void query(const char * query_string, std::function<void(IWbemClassObject*)> callback) const;
};

constexpr int CIM_UINT16LIST = 8210;

void variant_to_cpp_value(const void * variant, const long type, void * output_value);