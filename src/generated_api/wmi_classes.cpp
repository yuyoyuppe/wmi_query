
#include "pch.h"
#include "wmi_classes.h"

namespace wmi{
void Win32_FolderRedirectionHealth::deserialize(IWbemClassObject* const source, Win32_FolderRedirectionHealth& destination)
{
    fill_wmi_properties(source, 7,
      L"LastSuccessfulSyncTime", &destination.LastSuccessfulSyncTime,
      L"HealthStatus", &destination.HealthStatus,
      L"Redirected", &destination.Redirected,
      L"LastSyncStatus", &destination.LastSyncStatus,
      L"OfflineAccessEnabled", &destination.OfflineAccessEnabled,
      L"LastSyncTime", &destination.LastSyncTime,
      L"OfflineFileNameFolderGUID", &destination.OfflineFileNameFolderGUID);
}
std::vector<Win32_FolderRedirectionHealth> Win32_FolderRedirectionHealth::get_all_objects()
{
  std::vector<Win32_FolderRedirectionHealth> result;
  WMIProvider::get().query("select * from Win32_FolderRedirectionHealth", [&](IWbemClassObject* o, const WmiConnection&, const pugi::xml_document& doc) {
    Win32_FolderRedirectionHealth cpp_obj;
    Win32_FolderRedirectionHealth::deserialize(o, cpp_obj);
    result.emplace_back(std::move(cpp_obj));
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
void Win32_UserProfile::deserialize(IWbemClassObject* const source, Win32_UserProfile& destination)
{
    fill_wmi_properties(source, 16,
      L"RefCount", &destination.RefCount,
      L"LastUseTime", &destination.LastUseTime,
      L"HealthStatus", &destination.HealthStatus,
      L"LastUploadTime", &destination.LastUploadTime,
      L"LastAttemptedProfileDownloadTime", &destination.LastAttemptedProfileDownloadTime,
      L"LastDownloadTime", &destination.LastDownloadTime,
      L"LastAttemptedProfileUploadTime", &destination.LastAttemptedProfileUploadTime,
      L"LastBackgroundRegistryUploadTime", &destination.LastBackgroundRegistryUploadTime,
      L"Loaded", &destination.Loaded,
      L"LocalPath", &destination.LocalPath,
      L"RoamingConfigured", &destination.RoamingConfigured,
      L"Status", &destination.Status,
      L"SID", &destination.SID,
      L"RoamingPath", &destination.RoamingPath,
      L"RoamingPreference", &destination.RoamingPreference,
      L"Special", &destination.Special);
}
std::vector<Win32_UserProfile> Win32_UserProfile::get_all_objects()
{
  std::vector<Win32_UserProfile> result;
  WMIProvider::get().query("select * from Win32_UserProfile", [&](IWbemClassObject* o, const WmiConnection&, const pugi::xml_document& doc) {
    Win32_UserProfile cpp_obj;
    Win32_UserProfile::deserialize(o, cpp_obj);
    result.emplace_back(std::move(cpp_obj));
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

} //namespace wmi
