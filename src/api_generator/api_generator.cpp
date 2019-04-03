#include "pch.h"

#include <log.h>
#include <platform_error.h>
#include <utils.h>

#include <generator_common_constants.h>

using namespace wmi;

struct WMIProperty
{
  string_t _name;
  string_t _type;
};

struct WMIArrayProperty
{
  string_t              _name;
  string_t              _element_type;
  std::optional<size_t> _length;
};

namespace std
{
  template<> struct hash<WMIProperty>
  {
    using argument_type = WMIProperty;
    using result_type = std::size_t;
    result_type operator()(argument_type const& desc) const noexcept
    {
      return std::hash<string_t>{}(desc._name);
    }
  };

  template<> struct hash<WMIArrayProperty>
  {
    using argument_type = WMIArrayProperty;
    using result_type = std::size_t;
    result_type operator()(argument_type const& desc) const noexcept
    {
      return std::hash<string_t>{}(desc._name);
    }
  };
}

struct WMIClass
{
  string_t _name;
  std::unordered_set<WMIProperty>      _simple_properties;
  std::unordered_set<WMIArrayProperty> _array_properties;
  std::unordered_set<WMIProperty>      _object_properties;
};

namespace std
{
  template<> struct hash<WMIClass>
  {
    using argument_type = WMIClass;
    using result_type = std::size_t;
    result_type operator()(WMIClass const& desc) const noexcept
    {
      return std::hash<string_t>{}(desc._name);
    }
  };
}

bool operator==(const WMIClass& lhs, const WMIClass& rhs)
{
  return lhs._name == rhs._name;
}

bool operator==(const WMIProperty& lhs, const WMIProperty& rhs)
{
  return lhs._name == rhs._name;
}

bool operator==(const WMIArrayProperty& lhs, const WMIArrayProperty& rhs)
{
  return lhs._name == rhs._name;
}

template<typename... PropType>
void remove_length_properties(std::unordered_set<PropType>& ... prop_sets)
{
  const auto has_value_prop = [&](const string_t& prop_name_without_length)
  {
    return ((prop_sets.count(PropType{ prop_name_without_length, {} }) || ...));
  };

  const auto find_length_props = [&](auto& props)
  {
    for(auto it = begin(props); it != end(props);)
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
      if(has_value_prop(prop_name_without_length))
        it = props.erase(it);
      else
        ++it;
    }
    return 0;
  };
  int unfold[] = { find_length_props(prop_sets)... };
}

void type_str_to_cpp_type(std::string & type_str)
{
  const auto undecay = [&](const std::string_view decayed_type) -> std::string
  {
    return { cbegin(decayed_type), cend(decayed_type) };
  };

  if(type_str == "sint8") type_str = undecay("int8_t");
  else if(type_str == "uint8") type_str = undecay("uint8_t");
  else if(type_str == "sint16") type_str = undecay("int16_t");
  else if(type_str == "uint16") type_str = undecay("uint16_t");
  else if(type_str == "sint32") type_str = undecay("int32_t");
  else if(type_str == "uint32") type_str = undecay("uint32_t");
  else if(type_str == "sint64") type_str = undecay("int64_t");
  else if(type_str == "uint64") type_str = undecay("uint64_t");
  else if(type_str == "boolean") type_str = undecay("bool");
  else if(type_str == "real32") type_str = undecay("float");
  else if(type_str == "real64") type_str = undecay("double");
  else if(type_str == "string") type_str = undecay("std::string");
  else if(type_str == "char16") type_str = undecay("wchar_t");
  else if(type_str == "datetime") type_str = undecay("std::string");
  else
  {
    log(error, "got property of unknown type %s!", type_str.c_str());
    type_str = undecay(type_str);
  }
}

void unreserve_class_name(std::string& class_name)
{
  static const std::vector<const char*> reserved_strings = { "SuspendThread", "ResumeThread", "HeapFree", "HeapAlloc", "HeapCreate" };
  for(const auto& reserved_string : reserved_strings)
  {
    bool processed = false;
    while(class_name == reserved_string)
    {
      class_name += "_";
      processed = true;
    }
    if(processed)
      return;
  }
}

WMIClass build_wmi_class_description(const pugi::xml_document& doc)
{
  WMIClass desc;

  const auto xml_class = doc.child("CLASS");
  desc._name = xml_class.attribute("NAME").as_string();
  unreserve_class_name(desc._name);

  for(pugi::xml_node p : xml_class.children("PROPERTY"))
  {
    WMIProperty prop{ p.attribute("NAME").as_string(), p.attribute("TYPE").as_string() };
    type_str_to_cpp_type(prop._type);
    desc._simple_properties.emplace(std::move(prop));
  }
  for(pugi::xml_node o : xml_class.children("PROPERTY.OBJECT"))
  {
    desc._object_properties.emplace(WMIProperty{ o.attribute("NAME").as_string(), o.attribute("REFERENCECLASS").as_string() });
  }
  for(pugi::xml_node o : xml_class.children("PROPERTY.ARRAY"))
  {
    const size_t array_size = o.attribute("ARRAYSIZE").as_ullong();
    WMIArrayProperty prop{
      o.attribute("NAME").as_string(),
      o.attribute("TYPE").as_string(),
      array_size != 0 ? std::optional<size_t>{array_size} : std::nullopt };
    type_str_to_cpp_type(prop._element_type);
    desc._array_properties.emplace(std::move(prop));
  }
  remove_length_properties(desc._simple_properties, desc._array_properties);
  return desc;
}

std::unordered_set<WMIClass> build_wmi_classes_descriptions(const WMIProvider& provider)
{
  std::unordered_set<WMIClass> result;
  provider.query("select * from meta_class", [&](const pugi::xml_document& doc)
  {
    result.emplace(build_wmi_class_description(doc));
  });
  return result;
}

void generate_header_prologue(std::ostream& s)
{
  s << R"d(#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <array>
#include <pugixml.hpp>

namespace wmi{
)d";
}

void generate_source_prologue(std::ostream& s)
{
  s << R"(
#include "pch.h"
#include "wmi_classes.h"

namespace wmi{
)";
}


void generate_header_epilogue(std::ostream& s)
{
  s << R"(
} //namespace wmi
)";
}

void generate_source_epilogue(std::ostream& s)
{
  s << R"(
} //namespace wmi
)";
}

#define STRINGIFY(a) STRINGIFY_(a)
#define STRINGIFY_(a) #a
#define SV_ARG(name) fmt::arg(STRINGIFY(name), std::string_view{ name.data(), name.size() })

void generate_class_declaration(const WMIClass& class_desc, std::ostream& s)
{
  fmt::memory_buffer props;
  for(const auto& prop_info : class_desc._simple_properties)
    fmt::format_to(props, "  {} {};\n", prop_info._type, prop_info._name);

  fmt::memory_buffer array_props;
  for(const auto& array_prop_info : class_desc._array_properties)
  {
    const bool fixed_size = array_prop_info._length.has_value();
    fmt::format_to(array_props, fixed_size ? "  std::array<{}" : "  std::vector<{}", array_prop_info._element_type);
    if(fixed_size)
      fmt::format_to(array_props, ", {}", *array_prop_info._length);
    fmt::format_to(array_props, "> {};\n", array_prop_info._name);
  }

  fmt::memory_buffer object_props;
  for(const auto& prop_info : class_desc._object_properties)
    fmt::format_to(object_props, "  {} {};\n", prop_info._type, prop_info._name);

  fmt::memory_buffer result;
  fmt::format_to(result, R"(
struct {class_name}
{{
{props}
{array_props}
{object_props}
  static void deserialize(const pugi::xml_node& doc, {class_name}& destination);
  static std::vector<{class_name}> get_all_objects();
  std::string to_string() const;
}};
)", fmt::arg("class_name", class_desc._name), SV_ARG(props), SV_ARG(array_props), SV_ARG(object_props));
  s.write(result.data(), result.size());
}

fmt::memory_buffer generate_deserialize_func(const WMIClass& class_desc)
{
  fmt::memory_buffer props;
  for(const auto& prop_desc : class_desc._simple_properties)
  {
    fmt::format_to(props, R"(  Deserialize<{type}>::to(destination.{name}, 
    doc.select_node("PROPERTY[@NAME=\"{name}\"]/VALUE").node().text().as_string());
)", fmt::arg("type", prop_desc._type), fmt::arg("name", prop_desc._name));
  }
  fmt::memory_buffer object_props;
  for(const auto& obj_prop_desc : class_desc._object_properties)
  {
    fmt::format_to(object_props, R"(  {type}::deserialize(
    doc.select_node("INSTANCE/PROPERTY.OBJECT[@NAME=\"{name}\"]/INSTANCE").node(),            
    destination.{name});
)", fmt::arg("type", obj_prop_desc._type), fmt::arg("name", obj_prop_desc._name));
  }

  fmt::memory_buffer array_props;
  if(!class_desc._array_properties.empty())
    fmt::format_to(array_props, "  pugi::xpath_node_set nodes;\n");

  for(const auto& array_prop_desc : class_desc._array_properties)
  {
    const bool fixed_size = array_prop_desc._length.has_value();
    fmt::format_to(array_props, R"(
  nodes = doc.select_node("PROPERTY.ARRAY[@NAME=\"{}\"]/VALUE.ARRAY").node().select_nodes("VALUE");
)", array_prop_desc._name);

    if(!fixed_size)
      fmt::format_to(array_props, "  destination.{}.resize(nodes.size());\n", array_prop_desc._name);

    if(fixed_size)
      fmt::format_to(array_props, "  for(int i = 0; i < {}; ++i)\n", *array_prop_desc._length);
    else
      fmt::format_to(array_props, "  for(int i = 0; i < nodes.size(); ++i)\n");

    fmt::format_to(array_props, R"(    Deserialize<{type}>::to(destination.{name}[i], nodes[i].node().text().as_string());)", fmt::arg("type", array_prop_desc._element_type), fmt::arg("name", array_prop_desc._name));
  }

  fmt::memory_buffer deserialize;
  fmt::format_to(deserialize, R"(
void {class_name}::deserialize(const pugi::xml_node& doc, {class_name}& destination)
{{
{props}
{array_props}
{object_props}}}
)", fmt::arg("class_name", class_desc._name), SV_ARG(props), SV_ARG(array_props), SV_ARG(object_props));
  return deserialize;
}

fmt::memory_buffer generate_get_all_objects_func(const WMIClass& class_desc)
{
  fmt::memory_buffer query;
  fmt::format_to(query, R"(
std::vector<{class_name}> {class_name}::get_all_objects()
{{
  std::vector<{class_name}> result;
  WMIProvider::get().query("select * from {class_name}", [&](const pugi::xml_document& doc) {{
    {class_name} cpp_obj;
    {class_name}::deserialize(doc.child("INSTANCE"), cpp_obj);
    result.emplace_back(std::move(cpp_obj));
  }});
  return result;
}}
)", fmt::arg("class_name", class_desc._name));
  return query;
}

fmt::memory_buffer generate_to_string_func(const WMIClass& class_desc)
{
  fmt::memory_buffer func;
  const auto generate_to_string_for_simple_property = [](fmt::memory_buffer & b, const std::string_view name, const string_t& type)
  {
    const bool is_string = type == "std::string";
    const bool is_time = type == "std::time_t";
    if(is_string)
      fmt::format_to(b, name);
    else if(is_time)
      fmt::format_to(b, "asctime(localtime(&{})", name);
    else
      fmt::format_to(b, "std::to_string({})", name);
  };

  fmt::memory_buffer props;
  for(const auto&prop_desc : class_desc._simple_properties)
  {
    fmt::format_to(props, R"(  ((result += "{}: ") += )", prop_desc._name);
    generate_to_string_for_simple_property(props, prop_desc._name, prop_desc._type);
    fmt::format_to(props, ") += '\\n';\n");
  }

  fmt::memory_buffer array_props;
  for(const auto&array_prop_desc : class_desc._array_properties)
  {
    fmt::format_to(array_props, R"(
  result += "{name}: ";
  for(const auto & e : {name})
      (result += )", fmt::arg("name", array_prop_desc._name));
    generate_to_string_for_simple_property(array_props, "e", array_prop_desc._element_type);
    fmt::format_to(array_props, ") += ' ';\n");
    fmt::format_to(array_props, "  result += '\\n';\n");
  }

  fmt::memory_buffer object_props;
  for(const auto& obj_prop_desc : class_desc._object_properties)
  {
    fmt::format_to(object_props, R"(  ((result += "{name}: ") += {name}.to_string()) += '\n';
)", fmt::arg("name", obj_prop_desc._name));
  }

  fmt::format_to(func, R"(
std::string {class_name}::to_string() const
{{
  std::string result;
{props}
{array_props}
{object_props}
  return result;
}}
)", fmt::arg("class_name", class_desc._name), SV_ARG(props), SV_ARG(array_props), SV_ARG(object_props));
  return func;
}



void generate_class_definition(const WMIClass& class_desc, std::ostream& s)
{
  auto deserialize = generate_deserialize_func(class_desc);
  auto get_all_objects = generate_get_all_objects_func(class_desc);
  auto to_string = generate_to_string_func(class_desc);

  fmt::memory_buffer class_definition;
  fmt::format_to(class_definition, R"(
{deserialize}
{get_all_objects}
{to_string}
)", SV_ARG(deserialize), SV_ARG(get_all_objects), SV_ARG(to_string));
  s.write(class_definition.data(), class_definition.size());
}

auto build_class_declaration_order_and_remove_undefined_object_properties(const std::unordered_set<WMIClass>& unsorted_classes)
{
  std::vector<const WMIClass*> result;
  std::unordered_set<size_t> visited_classes;

  std::hash<WMIClass> hasher;

  auto visit = [&](auto & self, const WMIClass& class_desc, const size_t class_desc_hash)
  {
    if(visited_classes.count(class_desc_hash))
      return;

    for(auto it = begin(class_desc._object_properties); it != end(class_desc._object_properties);)
    {
      WMIClass to_search{ it->_type, {} };
      auto prop_class_desc = unsorted_classes.find(to_search);
      if(prop_class_desc != cend(unsorted_classes))
      {
        const WMIClass& v = *prop_class_desc;
        self(self, v, hasher(v));
        ++it;
      }
      else
      {
        // std::unordered_* containers give us const iters, so we don't change an item's hash without notice;
        // WMIClassDescription is hashed only by _name which we won't touch, so it's safe to cast here
        it = const_cast<WMIClass&>(class_desc)._object_properties.erase(it);
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

void filter_wmi_classes(std::unordered_set<WMIClass> & wmi_classes, const std::vector<const char*> & whitelist)
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

void generate_api(const WMIProvider& wmi_service, const std::unordered_set<WMIClass>& wmi_classes, std::ostream& header_file, std::ostream& source_file)
{
  const auto topology = build_class_declaration_order_and_remove_undefined_object_properties(wmi_classes);
  generate_header_prologue(header_file);
  generate_source_prologue(source_file);

  for(const auto class_desc : topology)
  {
    generate_class_declaration(*class_desc, header_file);
    generate_class_definition(*class_desc, source_file);
  }

  generate_header_epilogue(header_file);
  generate_source_epilogue(source_file);
}

int main()
{
  WMIProvider& wmi_service = WMIProvider::get();

  std::ofstream wmi_classes_header{ konst::wmi_path + "/wmi_classes.h"s };
  std::ofstream wmi_classes_source{ konst::wmi_path + "/wmi_classes.cpp"s };

  auto wmi_classes = build_wmi_classes_descriptions(wmi_service);
  //filter_wmi_classes(wmi_classes, { "WmiMonitorID" });
  //filter_wmi_classes(wmi_classes, { "Win32_UserProfile", "Win32_FolderRedirectionHealth" });

  generate_api(wmi_service, wmi_classes, wmi_classes_header, wmi_classes_source);
  wmi_service.uninitialize();
  return 0;
}