
#include "pch.h"
#include "wmi_classes.h"

std::vector<Win32_FolderRedirectionHealth> Win32_FolderRedirectionHealth::get_all_objects()
{
  std::vector<Win32_FolderRedirectionHealth> result;
  WMIProvider::get().query("select * from Win32_FolderRedirectionHealth", [&](IWbemClassObject* o, const WmiConnection&, const pugi::xml_document& doc) {
    VARIANT v; CIMTYPE type; Win32_FolderRedirectionHealth cpp_obj;
    o->Get(L"LastSuccessfulSyncTime", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.LastSuccessfulSyncTime);
    o->Get(L"HealthStatus", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.HealthStatus);
    o->Get(L"Redirected", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.Redirected);
    o->Get(L"LastSyncStatus", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.LastSyncStatus);
    o->Get(L"OfflineAccessEnabled", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.OfflineAccessEnabled);
    o->Get(L"LastSyncTime", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.LastSyncTime);
    o->Get(L"OfflineFileNameFolderGUID", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.OfflineFileNameFolderGUID);
    result.emplace_back(std::move(cpp_obj));VariantClear(&v);
  });
  return result;
}

std::string Win32_FolderRedirectionHealth::to_string() const
{
  std::ostringstream oss;
  oss << "LastSuccessfulSyncTime: " << (LastSuccessfulSyncTime) << std::endl;
  oss << "HealthStatus: " << std::to_string(HealthStatus) << std::endl;
  oss << "Redirected: " << std::to_string(Redirected) << std::endl;
  oss << "LastSyncStatus: " << std::to_string(LastSyncStatus) << std::endl;
  oss << "OfflineAccessEnabled: " << std::to_string(OfflineAccessEnabled) << std::endl;
  oss << "LastSyncTime: " << (LastSyncTime) << std::endl;
  oss << "OfflineFileNameFolderGUID: " << (OfflineFileNameFolderGUID) << std::endl;
  return oss.str();
}
std::vector<Win32_UserProfile> Win32_UserProfile::get_all_objects()
{
  std::vector<Win32_UserProfile> result;
  WMIProvider::get().query("select * from Win32_UserProfile", [&](IWbemClassObject* o, const WmiConnection&, const pugi::xml_document& doc) {
    VARIANT v; CIMTYPE type; Win32_UserProfile cpp_obj;
    o->Get(L"RefCount", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.RefCount);
    o->Get(L"LastUseTime", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.LastUseTime);
    o->Get(L"HealthStatus", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.HealthStatus);
    o->Get(L"LastUploadTime", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.LastUploadTime);
    o->Get(L"LastAttemptedProfileDownloadTime", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.LastAttemptedProfileDownloadTime);
    o->Get(L"LastDownloadTime", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.LastDownloadTime);
    o->Get(L"LastAttemptedProfileUploadTime", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.LastAttemptedProfileUploadTime);
    o->Get(L"LastBackgroundRegistryUploadTime", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.LastBackgroundRegistryUploadTime);
    o->Get(L"Loaded", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.Loaded);
    o->Get(L"LocalPath", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.LocalPath);
    o->Get(L"RoamingConfigured", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.RoamingConfigured);
    o->Get(L"Status", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.Status);
    o->Get(L"SID", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.SID);
    o->Get(L"RoamingPath", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.RoamingPath);
    o->Get(L"RoamingPreference", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.RoamingPreference);
    o->Get(L"Special", 0, &v, &type, nullptr);
    variant_to_cpp_value(&v, type, &cpp_obj.Special);
    result.emplace_back(std::move(cpp_obj));VariantClear(&v);
  });
  return result;
}

std::string Win32_UserProfile::to_string() const
{
  std::ostringstream oss;
  oss << "RefCount: " << std::to_string(RefCount) << std::endl;
  oss << "LastUseTime: " << (LastUseTime) << std::endl;
  oss << "HealthStatus: " << std::to_string(HealthStatus) << std::endl;
  oss << "LastUploadTime: " << (LastUploadTime) << std::endl;
  oss << "LastAttemptedProfileDownloadTime: " << (LastAttemptedProfileDownloadTime) << std::endl;
  oss << "LastDownloadTime: " << (LastDownloadTime) << std::endl;
  oss << "LastAttemptedProfileUploadTime: " << (LastAttemptedProfileUploadTime) << std::endl;
  oss << "LastBackgroundRegistryUploadTime: " << (LastBackgroundRegistryUploadTime) << std::endl;
  oss << "Loaded: " << std::to_string(Loaded) << std::endl;
  oss << "LocalPath: " << (LocalPath) << std::endl;
  oss << "RoamingConfigured: " << std::to_string(RoamingConfigured) << std::endl;
  oss << "Status: " << std::to_string(Status) << std::endl;
  oss << "SID: " << (SID) << std::endl;
  oss << "RoamingPath: " << (RoamingPath) << std::endl;
  oss << "RoamingPreference: " << std::to_string(RoamingPreference) << std::endl;
  oss << "Special: " << std::to_string(Special) << std::endl;
  return oss.str();
}
