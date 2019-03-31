#include "pch.h"

namespace wmi
{
  void fill_wmi_properties(IWbemClassObject* o, const int nProperties, ...)
  {
    va_list args;
    VARIANT v; CIMTYPE type;
    va_start(args, nProperties);
    for(int i = 0; i < nProperties * 2; i += 2)
    {
      const wchar_t* const property_name = va_arg(args, const wchar_t*);
      void* const property_destination = va_arg(args, void*);
      const bool ok = !FAILED(o->Get(property_name, 0, &v, &type, nullptr));
      if(ok)
      {
        variant_to_cpp_value(&v, type, property_destination);
        VariantClear(&v);
      }
    }
    va_end(args);
  }

}