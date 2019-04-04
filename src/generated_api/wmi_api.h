#include "pch.h"

namespace wmi
{
  template<typename Type, typename T = void>
  struct Deserialize;

  template<typename Type>
  struct Deserialize<Type, void>
  {
    static std::enable_if_t<std::is_arithmetic_v<Type>> to(Type& property_destination, const char* source)
    {
      if(auto [_, ec] = std::from_chars(source, source + strlen(source), property_destination); ec != std::errc())
        property_destination = {};
    }
  };

  template<>
  struct Deserialize<std::string>
  {
    static void to(std::string& property_destination, const char* source)
    {
      property_destination = source;
    }
  };

  template<>
  struct Deserialize<wchar_t>
  {
    static void to(wchar_t& property_destination, const char* source)
    {
      size_t res;
      mbstowcs_s(&res, &property_destination, 1, source, 1);  
    }
  };
  

  template<>
  struct Deserialize<bool>
  {
    static void to(bool& property_destination, const char* source)
    {
      property_destination = !strcmp("TRUE", source);
    }

    static void to(std::vector<bool>::reference property_destination, const char* source)
    {
      property_destination = !strcmp("TRUE", source);
    }
  };
}