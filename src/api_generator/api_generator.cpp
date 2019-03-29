#include "pch.h"

#include <log.h>
#include <platform_error.h>
#include <utils.h>

#include <generator_common_constants.h>

struct WMIClassProperty
{
  string_t _name;
  string_t _type;

};

namespace std
{
  template<> struct hash<WMIClassProperty>
  {
    using argument_type = WMIClassProperty;
    using result_type = std::size_t;
    result_type operator()(WMIClassProperty const& desc) const noexcept
    {
      return std::hash<string_t>{}(desc._name);
    }
  };
}

struct WMIClassDescription
{
  string_t _name;
  std::unordered_set<WMIClassProperty> _properties;
  std::unordered_set<WMIClassProperty> _object_properties;
};

namespace std
{
  template<> struct hash<WMIClassDescription>
  {
    using argument_type = WMIClassDescription;
    using result_type = std::size_t;
    result_type operator()(WMIClassDescription const& desc) const noexcept
    {
      return std::hash<string_t>{}(desc._name);
    }
  };
}

bool operator==(const WMIClassDescription& lhs, const WMIClassDescription& rhs)
{
  return lhs._name == rhs._name;
}

bool operator==(const WMIClassProperty& lhs, const WMIClassProperty& rhs)
{
  return lhs._name == rhs._name;
}

void remove_length_properties(WMIClassDescription& class_desc)
{
  auto & all_props = class_desc._properties;
  for(auto it = begin(all_props); it != end(all_props);)
  {
    const auto& prop_info = *it;
    static const char* suffix = "Length";
    static const size_t suffix_len = strlen(suffix);
    if(!ends_with(prop_info._name, suffix))
    {
      ++it;
      continue;
    }
    const auto prop_name_without_length = prop_info._name.substr(0, size(prop_info._name) - suffix_len);
    if(all_props.count(WMIClassProperty{ prop_name_without_length, {} }))
      it = all_props.erase(it);
    else
      ++it;
  }
}

void cof_type_string_to_cpp_type(WMIClassProperty & property)
{
  const bool is_array = ends_with(property._name, "[]");
  const auto undecay = [&](const std::string_view decayed_type) -> std::string
  {
    if(!is_array)
      return { cbegin(decayed_type), cend(decayed_type) };
    else
    {
      auto res = "std::vector<"s;
      res += decayed_type;
      res += ">";
      return res;
    }
  };

  if(property._type == "sint8") property._type = undecay("int8_t");
  else if(property._type == "uint8") property._type = undecay("uint8_t");
  else if(property._type == "sint16") property._type = undecay("int16_t");
  else if(property._type == "uint16") property._type = undecay("uint16_t");
  else if(property._type == "sint32") property._type = undecay("int32_t");
  else if(property._type == "uint32") property._type = undecay("uint32_t");
  else if(property._type == "sint64") property._type = undecay("int64_t");
  else if(property._type == "uint64") property._type = undecay("uint64_t");

  else if(property._type == "boolean") property._type = undecay("bool");

  else if(property._type == "real32") property._type = undecay("float");
  else if(property._type == "real64") property._type = undecay("double");

  else if(property._type == "string") property._type = undecay("std::string");
  else if(property._type == "datetime") property._type = undecay("std::string");
  else
  {
    log(error, "got property of unknown type %s!", property._type.c_str());
    property._type = undecay(property._type);
  }
}

void unreserve_class_name(std::string& class_name)
{
  static const std::vector<const char*> reserved_strings = { "SuspendThread", "ResumeThread", "HeapFree", "HeapAlloc", "HeapCreate" };
  for(const auto& reserved_string : reserved_strings)
    if(class_name == reserved_string)
    {
      class_name += "_";
      return;
    }
}

WMIClassDescription build_wmi_class_description(IWbemClassObject* pclsObj, const WmiConnection& connection, const pugi::xml_document& doc)
{
  WMIClassDescription desc;

  const auto xml_class = doc.child("CLASS");
  desc._name = xml_class.attribute("NAME").as_string();
  unreserve_class_name(desc._name);

  for(pugi::xml_node p : xml_class.children("PROPERTY"))
  {
    WMIClassProperty prop{ p.attribute("NAME").as_string(), p.attribute("TYPE").as_string() };
    cof_type_string_to_cpp_type(prop);
    desc._properties.emplace(std::move(prop));
  }
  for(pugi::xml_node n : xml_class.children("PROPERTY.OBJECT"))
  {
    desc._object_properties.emplace(WMIClassProperty{ n.attribute("NAME").as_string(), n.attribute("REFERENCECLASS").as_string() });
  }

  remove_length_properties(desc);
  return desc;
}

std::unordered_set<WMIClassDescription> build_wmi_classes_descriptions(const WMIProvider& provider)
{
  std::unordered_set<WMIClassDescription> result;
  provider.query("SELECT * FROM meta_class", [&](IWbemClassObject* pclsObj, const WmiConnection& connection, const pugi::xml_document& doc)
  {
    result.emplace(build_wmi_class_description(pclsObj, connection, doc));
  });
  return result;
}

// todo: don't use streams
void generate_header_preambule(std::ostream& s)
{
  s << R"d(#pragma once
#include <string>
#include <vector>
#include <chrono>
)d";
}

void generate_source_preambule(std::ostream& s)
{
  s << R"d(
#include "pch.h"
#include "wmi_classes.h"

)d";
}

void generate_class_declaration(const WMIClassDescription& class_desc, std::ostream& s)
{
  s << "struct " << class_desc._name << '\n';
  s << "{" << '\n';
  for(const auto& prop_info : class_desc._properties)
  {
    s << "  " << prop_info._type << " " << prop_info._name << ";\n";
  }
  if(size(class_desc._object_properties))
  {
    s << '\n' << "// object!\n";
    for(const auto& prop_info : class_desc._object_properties)
    {
      s << "  " << prop_info._type << " " << prop_info._name << ";\n";
    }
  }
  s << '\n';
  s << "  static std::vector<" << class_desc._name << "> get_all_objects();\n";
  s << "  std::string to_string() const;\n";
  s << "};\n\n";
}

void generate_class_definition(const WMIClassDescription& class_desc, std::ostream& s)
{
  s << "std::vector<" << class_desc._name << "> " << class_desc._name << "::get_all_objects()\n";
  s << "{\n";
  s << "  std::vector<" << class_desc._name << "> result;\n";
  s << "  WMIProvider::get().query(\"select * from " << class_desc._name << '"' << ", [&](IWbemClassObject* o, const WmiConnection&, const pugi::xml_document& doc) {\n";
  const bool has_properties = size(class_desc._properties);
  if(has_properties)
    s << "    VARIANT v; CIMTYPE type; ";
  s << class_desc._name << " cpp_obj;\n";
  for(const auto& prop_desc : class_desc._properties)
  {
    s << "    o->Get(L\"" << prop_desc._name << "\", 0, &v, &type, nullptr);\n";
    s << "    variant_to_cpp_value(&v, type, &cpp_obj." << prop_desc._name << ");\n";
  }
  s << "    result.emplace_back(std::move(cpp_obj));";
  if(has_properties)
    s << "VariantClear(&v);\n";
  s << "  });\n";
  s << "  return result;\n";
  s << "}\n\n";
  s << "std::string " << class_desc._name << "::to_string() const\n";
  s << "{\n";
  s << "  std::ostringstream oss;\n";
  for(const auto&prop_desc : class_desc._properties)
  {
    const bool is_string = prop_desc._type == "std::string";
    const bool is_time = prop_desc._type == "std::time_t";
    s << "  oss << \"" << prop_desc._name << ": \" << ";

    if(is_string)
    {
      s << "(" << prop_desc._name;
    }
    else if(is_time)
    {

      s << "asctime(localtime(&" << prop_desc._name << ")";
    }
    else
    {
      s << "std::to_string(" << prop_desc._name;
    }
    s << ") << std::endl;\n";
  }
  s << "  return oss.str();\n";
  s << "}\n";
}

auto get_wmi_classes_topology(const std::unordered_set<WMIClassDescription>& unsorted_classes)
{
  std::vector<const WMIClassDescription*> result;
  std::unordered_set<size_t> visited_classes; // permanent?

  std::hash<WMIClassDescription> hasher;

  auto visit = [&](auto & self, const WMIClassDescription& class_desc, const size_t class_desc_hash)
  {
    if(visited_classes.count(class_desc_hash))
      return;

    for(auto it = begin(class_desc._object_properties); it != end(class_desc._object_properties);)
    {
      WMIClassDescription to_search{ it->_type, {} };
      auto prop_class_desc = unsorted_classes.find(to_search);
      if(prop_class_desc != cend(unsorted_classes))
      {
        const WMIClassDescription& v = *prop_class_desc;
        self(self, v, hasher(v));
        ++it;
      }
      else
      {
        it = const_cast<WMIClassDescription&>(class_desc)._object_properties.erase(it);
      }
    }

    visited_classes.emplace(class_desc_hash);
    result.emplace_back(&class_desc);
  };

  for(auto & class_desc : unsorted_classes)
  {
    const size_t hash = hasher(class_desc);
    if(visited_classes.count(hash))
      continue;
    visit(visit, class_desc, hash);
  }
  return result;
}

void filter_wmi_classes(std::unordered_set<WMIClassDescription> & wmi_classes, const std::vector<const char*> & whitelist)
{
  for(auto it = begin(wmi_classes); it != end(wmi_classes);)
  {
    bool keep = false;
    for(const auto name : whitelist)
    {
      if(it->_name == name)
      {
        keep = true;
        break;
      }
    }
    if(keep)
      ++it;
    else
      it = wmi_classes.erase(it);
  }
}

void generate_api(const WMIProvider& wmi_service, std::ostream& header, std::ostream& source)
{
  auto wmi_classes = build_wmi_classes_descriptions(wmi_service);
  //debug
  filter_wmi_classes(wmi_classes, {"Win32_FolderRedirectionHealth", "Win32_UserProfile"});

  const auto topology = get_wmi_classes_topology(wmi_classes);
  generate_header_preambule(header);
  generate_source_preambule(source);

  for(const auto class_desc : topology)
  {
    generate_class_declaration(*class_desc, header);
    generate_class_definition(*class_desc, source);
  }
}

int main()
{
  namespace fs = std::filesystem;

  WMIProvider& wmi_service = WMIProvider::get();

  std::ofstream wmi_classes_header{ konst::wmi_path + "/wmi_classes.h"s };
  std::ofstream wmi_classes_source{ konst::wmi_path + "/wmi_classes.cpp"s };

  generate_api(wmi_service, wmi_classes_header, wmi_classes_source);
  return 0;
}