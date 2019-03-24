#include "pch.h"

#include <log.h>
#include <platform_error.h>
#include <utils.h>

#include <generator_common_constants.h>

struct WMIClassProperty
{
  string_t name;
  CIMTYPE type;
};

struct WMIClassDescription
{
  string_t _class_name;
  std::unordered_map<string_t, WMIClassProperty> _properties_by_name;
};

void remove_length_properties(WMIClassDescription& class_desc)
{
  auto & all_props = class_desc._properties_by_name;
  for(const auto&[prop_name, prop_info] : all_props)
  {
    static const char* suffix = "Length";
    static const size_t suffix_len = strlen(suffix);
    if(!endsWith(prop_name, suffix))
      continue;
    const auto prop_name_without_length = prop_name.substr(0, size(prop_name) - suffix_len);
    if(all_props.count(prop_name_without_length))
      all_props.erase(prop_name);
  }
}

std::unordered_map<string_t, WMIClassDescription> build_wmi_classes_list(const WMIProvider& provider)
{
  std::unordered_map<string_t, WMIClassDescription> result;
  provider.query("SELECT * FROM meta_class", [&](IWbemClassObject* pclsObj)
  {
    VARIANT v;
    HRESULT hr;
    CHECKED(pclsObj->Get(L"__CLASS", 0, &v, 0, 0));
    VariantClear(&v);
    const string_t class_name = wstring_to_string_t(V_BSTR(&v));

    WMIClassDescription& desc = result.emplace(class_name, WMIClassDescription{ class_name, {} }).first->second;
    SAFEARRAY * names;
    hr = pclsObj->GetNames(nullptr, WBEM_FLAG_ALWAYS | WBEM_FLAG_NONSYSTEM_ONLY, nullptr, &names);

    long iMin, iMax;
    SafeArrayGetLBound(names, 1, &iMin);
    SafeArrayGetUBound(names, 1, &iMax);
    IWbemObjectAccess* pAccess = nullptr;
    pclsObj->QueryInterface(IID_IWbemObjectAccess, reinterpret_cast<void**>(&pAccess));

    for(long i = iMin; i <= iMax; ++i)
    {
      BSTR elem = nullptr;
      SafeArrayGetElement(names, &i, &elem);
      CIMTYPE type;
      long handle;
      pAccess->GetPropertyHandle(elem, &type, &handle);
      const string_t property_name = wstring_to_string_t(elem);
      desc._properties_by_name.emplace(property_name, WMIClassProperty{ property_name, type });
      SysFreeString(elem);
    }
    remove_length_properties(desc);
    SafeArrayDestroy(names);
  });
  return result;
}


const char* cimtype_to_cpp_type_name(const CIMTYPE type)
{
  switch(type)
  {
  case CIM_SINT8:
    return "int8_t";
  case CIM_UINT8:
    return "uint8_t";
  case CIM_SINT16:
    return "int16_t";
  case CIM_UINT16:
    return "uint16_t";
  case CIM_SINT32:
    return "int32_t";
  case CIM_UINT32:
    return "uint32_t";
  case CIM_SINT64:
    return "int64_t";
  case CIM_UINT64:
    return "uint64_t";
  case CIM_REAL32:
    return "float";
  case CIM_REAL64:
    return "double";
  case CIM_BOOLEAN:
    return "bool";
  case CIM_STRING:
    return "std::string";
  case CIM_DATETIME:
    return "std::chrono::system_clock::time_point";
  case CIM_REFERENCE:
    return "void*";
  case CIM_CHAR16:
    return "wchar_t";

  case CIM_OBJECT:
  case CIM_FLAG_ARRAY:
  default:
    return "unknown";
  }
}

// todo: don't use streams
void generate_header_preambule(std::ostream& s)
{
  s << "#pragma once\n";
  s << "#include <string>\n";
  s << "#include <vector>\n";
}

void generate_source_preambule(std::ostream& s)
{
  s << "#include \"pch.h\"\n";
  s << "#include \"wmi_classes.h\"\n";
}

void generate_class_declaration(const WMIClassDescription& class_desc, std::ostream& s)
{
  s << "struct " << class_desc._class_name << '\n';
  s << "{" << '\n';
  for(const auto&[prop_name, prop_info] : class_desc._properties_by_name)
  {
    s << "  " << cimtype_to_cpp_type_name(prop_info.type) << " " << prop_name << ";\n";
  }
  s << '\n';
  s << "  static std::vector<" << class_desc._class_name << "> get_all_objects();\n";
  s << "  std::string to_string() const;\n";
  s << "};\n\n";
}

void generate_class_definition(const WMIClassDescription& class_desc, std::ostream& s)
{
  s << "std::vector<" << class_desc._class_name << "> " << class_desc._class_name << "::get_all_objects()\n";
  s << "{\n";
  s << "  std::vector<" << class_desc._class_name << "> result;\n";
  s << "  WMIProvider::get().query(\"select * from " << class_desc._class_name << '"' << ", [&](IWbemClassObject* o) {\n";
  s << "    VARIANT v; CIMTYPE type; " << class_desc._class_name << " cpp_obj;\n";
  for(const auto&[prop_name, prop_info] : class_desc._properties_by_name)
  {
    s << "    o->Get(L\"" << prop_name << "\", 0, &v, &type, nullptr);\n";
    s << "    variant_to_cpp_value(&v, type, &cpp_obj." << prop_name << ");\n";
  }
  s << "    result.emplace_back(std::move(cpp_obj)); VariantClear(&v);\n";
  s << "  });\n";
  s << "  return result;\n";
  s << "}\n\n";
  s << "std::string " << class_desc._class_name << "::to_string() const\n";
  s << "{\n";
  s << "  std::ostringstream oss;\n";
  for(const auto&[prop_name, prop_info] : class_desc._properties_by_name)
  {
    const bool is_string = prop_info.type == CIM_STRING;
    s << "  oss << \"" << prop_name << ": \" << " << (is_string? "(" : "std::to_string(") << prop_name << ") << std::endl;\n";
  }
  s << "  return oss.str();\n";
  s << "}\n";
}

void generate_api(const WMIProvider& wmi_service, std::ostream& header, std::ostream& source)
{
  const auto wmi_classes = build_wmi_classes_list(wmi_service);
  const auto& class_desc = wmi_classes.at("WmiMonitorID");

  generate_header_preambule(header);
  generate_source_preambule(source);

  generate_class_declaration(class_desc, header);
  generate_class_definition(class_desc, source);
}

int main()
{
  using namespace std::string_literals;
  namespace fs = std::filesystem;

  WMIProvider& wmi_service = WMIProvider::get();
  std::ofstream wmi_classes_header{konst::wmi_path + "/wmi_classes.h"s};
  std::ofstream wmi_classes_source{konst::wmi_path + "/wmi_classes.cpp"s};
  
  generate_api(wmi_service, wmi_classes_header, wmi_classes_source);
  return 0;
}