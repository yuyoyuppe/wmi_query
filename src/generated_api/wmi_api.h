#include "pch.h"
#include <iostream>
namespace wmi
{
  void deserialize_wmi_properties(IWbemClassObject* o, const int nProperties, ...);

  template<typename ObjectT>
  void deserialize_wmi_object_property(IWbemClassObject* parent_object, const char* parent_object_type, const char* object_type, ObjectT& destination)
  {
    VARIANT v; CIMTYPE type;
    parent_object->Get(L"__PATH", 0, &v, &type, nullptr);
    string_t query_str;
    variant_to_cpp_value(&v, type, &query_str);
    std::string search_str{":"};
    search_str += parent_object_type;
    search_str += '.';
    query_str = std::string{"select "} + object_type + " from " + parent_object_type + " where " +
      query_str.substr(query_str.find(search_str) + size(search_str), size(query_str) - size(search_str));
    WMIProvider::get().query(query_str.c_str(), [&](IWbemClassObject* o, const WmiConnection&, const pugi::xml_document& doc)
    {
      ObjectT::deserialize(o, destination);
    });
    VariantClear(&v);

  }
}