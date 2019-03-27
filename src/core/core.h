#pragma once
#include "config.h"

#include <functional>
#include <vector>
#include <pugixml.hpp>

struct IWbemLocator;
struct IWbemServices;
struct IWbemClassObject;

string_t wstring_to_string_t(const std::wstring & ws);

struct WmiConnection
{
    IWbemLocator * _locator = nullptr;
    IWbemServices * _services = nullptr;
    IWbemContext * _context = nullptr;
};

class WMIProvider
{

  std::vector<WmiConnection> _connections;
  WMIProvider(const std::vector<std::wstring_view>& namespaces_to_use = {L"ROOT\\wmi", L"Root\\CIMV2"});
public:
  ~WMIProvider();
  static WMIProvider& get();
  void query(const char * query_string, std::function<void(IWbemClassObject*, const WmiConnection&, const pugi::xml_document&)> callback) const;

  static IWbemContext * CreateContext(const int pathLevel = 0); //for debug
};

constexpr int CIM_UINT16LIST = 8210;

void variant_to_cpp_value(const void * variant, const long type, void * output_value);