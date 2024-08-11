#pragma once
#include "config.h"

#include <memory>
#include <functional>
#include <vector>
#include <pugixml.hpp>


namespace wmi{
string_t wstring_to_string_t(const std::wstring & ws);

class WMIProvider
{
  struct impl;
  std::unique_ptr<impl> _impl;
  
  WMIProvider(const std::vector<std::wstring_view>& namespaces_to_use = {L"ROOT\\wmi", L"Root\\CIMV2"});
public:
  
  static WMIProvider& get();
  static void uninitialize();

  void query(const char * query_string, std::function<void(const pugi::xml_document&)> callback) const;
};

}