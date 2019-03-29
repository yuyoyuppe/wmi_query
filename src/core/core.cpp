#include "pch.h"

#include "core.h"

WMIProvider& WMIProvider::get()
{
  static WMIProvider instance;
  return instance;
}

IWbemContext * WMIProvider::CreateContext(const int pathLevel)
{
    IWbemContext *context = nullptr;
    CHECKED(CoCreateInstance(CLSID_WbemContext,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_IWbemContext,
        (void**)&context));

    VARIANT vValue;

    VariantInit(&vValue);
    vValue.vt = VT_BOOL;
    vValue.boolVal = VARIANT_TRUE;
    context->SetValue(L"IncludeQualifiers", 0, &vValue);
    VariantClear(&vValue);

    VariantInit(&vValue);
    vValue.vt = VT_I4;
    vValue.lVal = pathLevel;
    context->SetValue(L"PathLevel", 0, &vValue);
    VariantClear(&vValue);

    VariantInit(&vValue);
    vValue.vt = VT_BOOL;
    vValue.boolVal = VARIANT_TRUE;
    context->SetValue(L"ExcludeSystemProperties", 0, &vValue);
    VariantClear(&vValue);

    VariantInit(&vValue);
    vValue.vt = VT_BOOL;
    vValue.lVal = VARIANT_TRUE;
    context->SetValue(L"LocalOnly", 0, &vValue);
    VariantClear(&vValue);

    return context;
}


WMIProvider::WMIProvider(const std::vector<std::wstring_view>& namespaces_to_use)
{
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
  _connections.reserve(size(namespaces_to_use));
  for(const auto& namespace_to_use : namespaces_to_use)
  {
    IWbemLocator * locator = nullptr;
    IWbemServices * services = nullptr;
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
    IWbemContext *context = CreateContext();
    _connections.emplace_back(WmiConnection{locator, services, context});
  }
}

WMIProvider::~WMIProvider()
{
  for(const auto& c : _connections)
  {
    if(c._services)
      c._services->Release();
    if(c._locator)
      c._locator->Release();
  }

  CoUninitialize();
}
void WMIProvider::query(const char * query_string, std::function<void(IWbemClassObject*, const WmiConnection&, const pugi::xml_document&)> callback) const
{
  for(const auto& connection : _connections)
  {
    IEnumWbemClassObject* pEnumerator = nullptr;
    CHECKED(connection._services->ExecQuery(
      bstr_t("WQL"),
      bstr_t(query_string),
      WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
      nullptr,
      &pEnumerator));
    IWbemClassObject *pclsObj = nullptr;
    ULONG uReturn = 0;
    while(pEnumerator)
    {
      HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
        &pclsObj, &uReturn);

      if(0 == uReturn)
      {
        break;
      }
      IWbemObjectTextSrc *objTextSrc = nullptr;

      CHECKED(CoCreateInstance(CLSID_WbemObjectTextSrc,
          nullptr,
          CLSCTX_INPROC_SERVER,
          IID_IWbemObjectTextSrc,
          (void**)&objTextSrc));
      BSTR objText;
      CHECKED(objTextSrc->GetText(0,
          pclsObj,
          WMI_OBJ_TEXT_CIM_DTD_2_0,
          connection._context,
          &objText));
      const auto xml_buf_len = SysStringLen(objText) * sizeof(wchar_t);
      pugi::xml_document doc;
      CHECKED(doc.load_buffer_inplace(objText, xml_buf_len, 116U, pugi::encoding_utf16_le));
      callback(pclsObj, connection, doc);
      pclsObj->Release();
      SysFreeString(objText);
    }
    pEnumerator->Release();

  }
}

string_t wstring_to_string_t(const std::wstring & ws)
{
  return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>{}.to_bytes(ws);
}

void variant_to_cpp_value(const void* v_untyped, const long type, void * output_value)
{
  const VARIANT& v = *static_cast<const VARIANT*>(v_untyped);
  const bool empty = v.vt < 2;
    switch(type)
    {
    case CIM_SINT8:
      *reinterpret_cast<int8_t*>(output_value) = v.cVal;
      return;
    case CIM_SINT16:
      *reinterpret_cast<int16_t*>(output_value) = v.iVal;
      return;
    case CIM_SINT32:
      *reinterpret_cast<int32_t*>(output_value) = v.intVal;
      return;
    case CIM_SINT64:
      *reinterpret_cast<int64_t*>(output_value) = v.lVal;
      return;
    case CIM_UINT8:
      *reinterpret_cast<uint8_t*>(output_value) = v.bVal;
      return;
    case CIM_UINT16:
      *reinterpret_cast<uint16_t*>(output_value) = v.uiVal;
      return;
    case CIM_UINT32:
      *reinterpret_cast<uint32_t*>(output_value) = v.uintVal;
      return;
    case CIM_UINT64:
      *reinterpret_cast<uint64_t*>(output_value) = v.ulVal;
      return;
    case CIM_REAL32:
      *reinterpret_cast<float*>(output_value) = v.fltVal;
      return;
    case CIM_REAL64:
      *reinterpret_cast<double*>(output_value) = v.dblVal;
      return;
    case CIM_BOOLEAN:
      *reinterpret_cast<bool*>(output_value) = v.boolVal == VARIANT_TRUE;
      return;
    case CIM_STRING:
    case CIM_DATETIME: // don't assume user preference :)
      *reinterpret_cast<string_t*>(output_value) = empty? "" : wstring_to_string_t(v.bstrVal);
      return;
    case CIM_UINT16LIST:
    {
      std::string val;
      long iMin, iMax;
      SafeArrayGetLBound(v.parray, 1, &iMin);
      SafeArrayGetUBound(v.parray, 1, &iMax);
      val.resize(iMax - iMin + 1);
      int idx = 0;
#ifdef _DEBUG
      VARTYPE elem_type;
      SafeArrayGetVartype(v.parray, &elem_type);
#endif
      for(long i = iMin; i <= iMax; ++i)
      {
        SafeArrayGetElement(v.parray, &i, &val[idx]);
        if(val[idx++] == '\0')
          break;
      }
      val.resize(idx - 1);
      *reinterpret_cast<std::string*>(output_value) = std::move(val);

    }
    case CIM_REFERENCE:
      // todo
      break;
    case CIM_CHAR16:
      // todo
      //result.emplace<>(v.pdispVal)
      break;
    case CIM_OBJECT:
    case CIM_FLAG_ARRAY:
    default:
      log(error, "unknown variant type:%ld", type);
      break;
    }
}
