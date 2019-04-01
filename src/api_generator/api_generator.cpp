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
  int unfold[] = {find_length_props(prop_sets)...};
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
  s << R"d(
#include "pch.h"
#include "wmi_classes.h"

namespace wmi{
)d";
}


void generate_header_epilogue(std::ostream& s)
{
  s << R"d(
} //namespace wmi
)d";
}

void generate_source_epilogue(std::ostream& s)
{
  s << R"d(
} //namespace wmi
)d";
}

constexpr const char * class_definition_t = R"(
void ${class_name}::deserialize(const pugi::xml_node& doc, ${class_name}& destination)
{
@prop_desc
{{
  Deserialize<${_type}>::to(destination.${_name}, 
    doc.select_node("PROPERTY[@NAME=\"${_name}\"]/VALUE").node().text().as_string());
}}

@obj_prop_desc
{{
  ${{_type}}::deserialize(
    doc.select_node("INSTANCE/PROPERTY.OBJECT[@NAME=\"${_name}\"]/INSTANCE").node(),            
    destination.${_name});
}}

??has_array_properties
{{
  pugi::xpath_node_set nodes;
}}

@array_prop_desc
{{

}}

)";

namespace fmt
{
  auto arg(const char* name, const memory_buffer& buf)
  {
    static std::string empty = "";
    return buf.size()? arg(name, std::string_view{buf.data(), buf.size()}) : arg(name, std::string_view{empty});
  }
}

void generate_class_declaration(const WMIClass& class_desc, std::ostream& s)
{
  fmt::memory_buffer props;
  for(const auto& prop_info : class_desc._simple_properties)
    fmt::format_to(props, "  {} {};\n", prop_info._type, prop_info._name);
  
  fmt::memory_buffer array_props;
  for(const auto& array_prop_info : class_desc._array_properties)
  {
    const bool fixed_size = array_prop_info._length.has_value();
    const char* container_type = fixed_size? "std::array<" : "std::vector<";
    fmt::format_to(array_props, "  {} {}", container_type, array_prop_info._element_type);
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
  static std::vector<{class_name}> get_all_objects();
  std::string to_string() const;
  static void deserialize(const pugi::xml_node& doc, {class_name}& destination);
}};
)", fmt::arg("class_name", class_desc._name), fmt::arg("props", props), fmt::arg("array_props", array_props), fmt::arg("object_props", object_props));
  s << std::string_view{result.data(), result.size()};
}

void generate_class_definition(const WMIClass& class_desc, std::ostream& s)
{
  // deserialize
  s << "void " << class_desc._name << "::deserialize(const pugi::xml_node& doc, " << class_desc._name << "& destination)\n";
  s << "{\n";
  for(const auto& prop_desc : class_desc._simple_properties)
  {
    s << "  Deserialize<" << prop_desc._type << ">::to(destination." << prop_desc._name << R"(, doc.select_node("PROPERTY[@NAME=\")" << prop_desc._name << "\\\"]/VALUE\").node().text().as_string());\n";
  }

  for(const auto& obj_prop_desc : class_desc._object_properties)
  {
    s << "  " << obj_prop_desc._type << "::deserialize(" << R"(doc.select_node("INSTANCE/PROPERTY.OBJECT[@NAME=\")" << obj_prop_desc._name << R"(\"]/INSTANCE").node(), destination.)" << obj_prop_desc._name << ");\n";
  }

  if(!class_desc._array_properties.empty())
    s << "  pugi::xpath_node_set nodes;\n";

  for(const auto& array_prop_desc : class_desc._array_properties)
  {
    const bool fixed_size = array_prop_desc._length.has_value();
    s << R"(  nodes = doc.select_node("PROPERTY.ARRAY[@NAME=\")" << array_prop_desc._name << R"(\"]/VALUE.ARRAY").node().select_nodes("VALUE");)" << '\n';
    if(!fixed_size)
      s << "  destination." << array_prop_desc._name << ".resize(nodes.size());\n";
    s << "  for(int i = 0; i < "; 
    if(fixed_size) 
      s << *array_prop_desc._length; 
    else
      s << "nodes.size()";
    s << "; ++i)\n";

    s << "    Deserialize<" << array_prop_desc._element_type << ">::to(destination." << array_prop_desc._name << "[i], nodes[i].node().text().as_string());\n";
  }
  s << "}\n\n";

  // query
  s << "std::vector<" << class_desc._name << "> " << class_desc._name << "::get_all_objects()\n";
  s << "{\n";
  s << "  std::vector<" << class_desc._name << "> result;\n";
  s << "  WMIProvider::get().query(\"select * from " << class_desc._name << '"' << ", [&](const pugi::xml_document& doc) {\n";
  s << "    " << class_desc._name << " cpp_obj;\n";
  s << "    " << class_desc._name << "::deserialize(doc.child(\"INSTANCE\"), cpp_obj);\n";
  s << "    result.emplace_back(std::move(cpp_obj));\n";
  s << "  });\n";
  s << "  return result;\n";

  s << "}\n\n";

  // to_string
  s << "std::string " << class_desc._name << "::to_string() const\n";
  s << "{\n";
  s << "  std::ostringstream oss;\n";

  const auto generate_to_string_for_simple_property = [&s](const std::string_view name, const string_t& type)
  {
    const bool is_string = type == "std::string";
    const bool is_time = type == "std::time_t";
    if(is_string)
      s << '(' << name << ')';
    else if(is_time)
      s << "asctime(localtime(&" << name << ')';
    else
      s << "std::to_string(" << name << ')';
  };

  for(const auto&prop_desc : class_desc._simple_properties)
  {
    s << "  oss << \"" << prop_desc._name << ": \" << ";
    generate_to_string_for_simple_property(prop_desc._name, prop_desc._type);
    s << " << std::endl;\n";
  }
  for(const auto&array_prop_desc : class_desc._array_properties)
  {
    s << "  oss << \"" << array_prop_desc._name << ": \";\n";
    s << "  for(const auto & e : " << array_prop_desc._name << ")\n";
    s << "    oss << "; generate_to_string_for_simple_property("e", array_prop_desc._element_type); 
    s << " << ' ';\n";
    s << "  oss << std::endl;\n";
  }

  for(const auto& obj_prop_desc : class_desc._object_properties)
  {
    s << "  oss << \"" << obj_prop_desc._name << ": \" << " << obj_prop_desc._name << ".to_string() << std::endl;\n";
  }

  s << "  return oss.str();\n";
  s << "}\n";
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
  filter_wmi_classes(wmi_classes, {"WmiMonitorID"});
  //filter_wmi_classes(wmi_classes, { "Win32_UserProfile", "Win32_FolderRedirectionHealth" });

  generate_api(wmi_service, wmi_classes, wmi_classes_header, wmi_classes_source);
  wmi_service.uninitialize();
  return 0;
}