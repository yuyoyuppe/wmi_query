#include "pch.h"

#include "core.h"

_COM_SMARTPTR_TYPEDEF(IWbemServices, IID_IWbemServices); 
_COM_SMARTPTR_TYPEDEF(IWbemLocator, IID_IWbemLocator);
_COM_SMARTPTR_TYPEDEF(IWbemContext, IID_IWbemContext);
_COM_SMARTPTR_TYPEDEF(IEnumWbemClassObject, IID_IEnumWbemClassObject);
_COM_SMARTPTR_TYPEDEF(IWbemClassObject, IID_IWbemClassObject);
_COM_SMARTPTR_TYPEDEF(IWbemObjectTextSrc, IID_IWbemObjectTextSrc);

namespace wmi{
struct WmiConnection
{
  IWbemLocatorPtr _locator;
  IWbemServicesPtr _services;
  IWbemContextPtr _context;
};

struct WMIProvider::impl
{
  std::vector<WmiConnection> _connections;
};

WMIProvider* instance = nullptr;
std::once_flag instance_initialized;

WMIProvider& WMIProvider::get()
{
  std::call_once(instance_initialized, [&]
  {
    instance = new WMIProvider();
  });

  return *instance;
}

void WMIProvider::uninitialize()
{
  if(instance)
  {
    delete instance;
    CoUninitialize();
  }
}

IWbemContextPtr CreateContext()
{
    IWbemContextPtr context;
    CHECKED(CoCreateInstance(CLSID_WbemContext,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_IWbemContext,
        (void**)&context));

    _variant_t vValue;

    vValue.vt = VT_BOOL;
    vValue.boolVal = VARIANT_FALSE;
    context->SetValue(L"IncludeQualifiers", 0, &vValue);
    vValue.Clear();

    vValue.vt = VT_I4;
    vValue.lVal = 0;
    context->SetValue(L"PathLevel", 0, &vValue);
    vValue.Clear();

    vValue.vt = VT_BOOL;
    vValue.boolVal = VARIANT_TRUE;
    context->SetValue(L"ExcludeSystemProperties", 0, &vValue);
    vValue.Clear();

    vValue.vt = VT_BOOL;
    vValue.lVal = VARIANT_TRUE;
    context->SetValue(L"LocalOnly", 0, &vValue);
    vValue.Clear();

    return context;
}


WMIProvider::WMIProvider(const std::vector<std::wstring_view>& namespaces_to_use)
{
  _impl = std::make_unique<impl>();

  CHECKED(CoInitializeEx(0, COINIT_MULTITHREADED));

  CHECKED(CoInitializeSecurity(
    nullptr,
    -1,
    nullptr,
    nullptr,
    RPC_C_AUTHN_LEVEL_DEFAULT,
    RPC_C_IMP_LEVEL_IMPERSONATE,
    nullptr,
    EOAC_NONE,
    nullptr
  ));
  _impl->_connections.reserve(size(namespaces_to_use));
  for(const auto& namespace_to_use : namespaces_to_use)
  {
    IWbemLocatorPtr locator;
    IWbemServicesPtr services;
    CHECKED(CoCreateInstance(
      CLSID_WbemLocator,
      0,
      CLSCTX_INPROC_SERVER,
      IID_IWbemLocator, (LPVOID *)&locator));

    CHECKED(locator->ConnectServer(
      _bstr_t(namespace_to_use.data()),
      nullptr,
      0,
      nullptr,
      0,
      nullptr,
      nullptr,
      &services
    ));

    CHECKED(CoSetProxyBlanket(
      services,
      RPC_C_AUTHN_WINNT,
      RPC_C_AUTHZ_NONE,
      nullptr,
      RPC_C_AUTHN_LEVEL_CALL,
      RPC_C_IMP_LEVEL_IMPERSONATE,
      nullptr,
      EOAC_NONE
    ));
    auto context = CreateContext();
    _impl->_connections.emplace_back(WmiConnection{locator, services, context});
  }
}

void WMIProvider::query(const char * query_string, std::function<void(const pugi::xml_document&)> callback) const
{
  for(const auto& connection : _impl->_connections)
  {
    IEnumWbemClassObjectPtr enum_class_obj;
    CHECKED(connection._services->ExecQuery(
      bstr_t("WQL"),
      bstr_t(query_string),
      WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
      nullptr,
      &enum_class_obj));
    IWbemClassObjectPtr class_obj;
    ULONG ret = 0;
    while(enum_class_obj)
    {
      enum_class_obj->Next(WBEM_INFINITE, 1,
        &class_obj, &ret);

      if(0 == ret)
        break;
      IWbemObjectTextSrcPtr obj_text_src;

      CHECKED(CoCreateInstance(CLSID_WbemObjectTextSrc,
          nullptr,
          CLSCTX_INPROC_SERVER,
          IID_IWbemObjectTextSrc,
          (void**)&obj_text_src));
      
      _bstr_t obj_text;
      CHECKED(obj_text_src->GetText(0,
          class_obj,
          WMI_OBJ_TEXT_CIM_DTD_2_0,
          connection._context,
          obj_text.GetAddress()));
      const auto xml_buf_len = obj_text.length() * sizeof(wchar_t);
      pugi::xml_document doc;
      CHECKED((doc.load_buffer_inplace(static_cast<wchar_t*>(obj_text), xml_buf_len, 116U, pugi::encoding_utf16_le)));
      callback(doc);
    }
  }
}

string_t wstring_to_string_t(const std::wstring & ws)
{
  return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>{}.to_bytes(ws);
}
}