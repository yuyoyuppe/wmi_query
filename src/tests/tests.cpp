#include "pch.h"

#include <log.h>

#include <wmi_classes.h>

int mzain()
{
  const auto objects = wmi::WmiMonitorID::get_all_objects();
  for(const auto& o : objects)
    log(info, "got: %s", o.to_string().c_str());
  
  return 0;
}



#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Data.Xml.Dom.h>

#include <xmllite.h>
#include <string_view>
#include <variant>
#include <sstream>

std::wstring parse_utf16_value_array(const winrt::Windows::Data::Xml::Dom::IXmlNode & value_array_node)
{
  std::wstring result;
  for(auto char_node = value_array_node.FirstChild(); char_node; char_node = char_node.NextSibling())
  {
    uint16_t char_code;
    std::wistringstream{std::wstring{char_node.InnerText()}} >> char_code;
    result += char_code;
  }
  return result;
}

#include <wil/com.h>
#include <wrl/implements.h>
#define _WIN32_DCOM
#include <comdef.h>
#include <Wbemidl.h>


wil::com_ptr_t<IWbemContext> setup_wmi_context()
{
  auto context = wil::CoCreateInstance<WbemContext, IWbemContext>(CLSCTX_INPROC_SERVER);
  _variant_t option;

  option.vt = VT_BOOL;
  option.boolVal = VARIANT_FALSE;
  context->SetValue(L"IncludeQualifiers", 0, &option);
  option.Clear();

  option.vt = VT_I4;
  option.lVal = 0;
  context->SetValue(L"PathLevel", 0, &option);
  option.Clear();

  option.vt = VT_BOOL;
  option.boolVal = VARIANT_TRUE;
  context->SetValue(L"ExcludeSystemProperties", 0, &option);
  option.Clear();

  option.vt = VT_BOOL;
  option.lVal = VARIANT_FALSE;
  context->SetValue(L"LocalOnly", 0, &option);
  option.Clear();

  return context;
}

struct WmiConnection
{
  wil::com_ptr_t<IWbemServices> _services;
  wil::com_ptr_t<IWbemLocator>  _locator;
  wil::com_ptr_t<IWbemContext>  _context;
};


WmiConnection initialize_wmi_connection()
{
  WmiConnection con;
  con._locator = wil::CoCreateInstance<WbemLocator, IWbemLocator>(CLSCTX_INPROC_SERVER);
  con._context = setup_wmi_context();

  if(FAILED(con._locator->ConnectServer(
    L"ROOT\\wmi",
    nullptr,
    nullptr,
    nullptr,
    0,
    nullptr,
    nullptr,
    &con._services)))
  {
    throw std::runtime_error("cannot initialize WMI!");
  }

  if(FAILED(CoSetProxyBlanket(
    con._services.get(),
    RPC_C_AUTHN_WINNT,
    RPC_C_AUTHZ_NONE,
    nullptr,
    RPC_C_AUTHN_LEVEL_CALL,
    RPC_C_IMP_LEVEL_IMPERSONATE,
    nullptr,
    EOAC_NONE
  )))
  {
    throw std::runtime_error("cannot initialize WMI!");
  }
  return con;
}

struct WmiMonitorID
{
  std::wstring _friendly_name;
  std::wstring _serial_number_id;
};

std::optional<WmiMonitorID> parse_monitorID(const std::wstring & buffer)
{
  try
  {
    winrt::Windows::Data::Xml::Dom::XmlDocument xml_doc;
    winrt::Windows::Data::Xml::Dom::XmlLoadSettings load_settings;
    load_settings.ValidateOnParse(false);
    load_settings.ElementContentWhiteSpace(false);
    xml_doc.LoadXml(buffer, load_settings);
    xml_doc.Normalize();
    WmiMonitorID result;
    for(const auto & node : xml_doc.GetElementsByTagName(L"PROPERTY.ARRAY"))
    {
      for(const auto & attr : node.Attributes())
      {
        if(attr.NodeName() != L"NAME")
        {
          continue;
        }
        const auto property_name = attr.InnerText();

        if(property_name == L"UserFriendlyName")
        {
          result._friendly_name = parse_utf16_value_array(node.FirstChild());
        }
        else if(property_name == L"SerialNumberID")
        {
          result._serial_number_id = parse_utf16_value_array(node.FirstChild());
        }
      }
    }
    return result;
  }
  catch(winrt::hresult_error const & /*ex*/)
  {
    //winrt::hresult hr = ex.to_abi();
    //winrt::hstring message = ex.message();
    //const XmlError hex = (XmlError)hr.value;
  }
  catch(...)
  {
  }
  return std::nullopt;
}

int test()
{
  winrt::init_apartment();

  auto connection = initialize_wmi_connection();
  wil::com_ptr<IEnumWbemClassObject> enum_class_obj;
  if(FAILED(connection._services->ExecQuery(
    bstr_t("WQL"),
    bstr_t("select * from WmiMonitorID"),
    WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
    nullptr,
    &enum_class_obj)))
  {
    throw std::runtime_error("WMI query error!");
  }
  wil::com_ptr_t<IWbemClassObject> class_obj;
  ULONG next_res{};
  while(enum_class_obj)
  {
    enum_class_obj->Next(WBEM_INFINITE, 1, &class_obj, &next_res);

    if(!next_res)
    {
      break;
    }
    auto obj_text_src = wil::CoCreateInstance<WbemObjectTextSrc, IWbemObjectTextSrc>(CLSCTX_INPROC_SERVER);
    _bstr_t obj_text;
    if(FAILED(obj_text_src->GetText(0,
      class_obj.get(),
      WMI_OBJ_TEXT_CIM_DTD_2_0,
      connection._context.get(),
      obj_text.GetAddress())))
    {
      throw std::runtime_error("WMI query error!");
    }
    const auto xml_buf_len = obj_text.length() * sizeof(wchar_t);
    auto monitordata = parse_monitorID(static_cast<const wchar_t*>(obj_text));
  }

  return 0;
}


int main()
{
  return test();

}