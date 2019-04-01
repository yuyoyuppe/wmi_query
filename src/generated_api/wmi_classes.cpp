
#include "pch.h"
#include "wmi_classes.h"

namespace wmi{
void Win32_FolderRedirectionHealth::deserialize(const pugi::xml_document& doc, Win32_FolderRedirectionHealth& destination)
{
  Deserialize<std::string>::to(destination.LastSuccessfulSyncTime, doc.select_node("INSTANCE/PROPERTY[@NAME=\"LastSuccessfulSyncTime\"]/VALUE").node().text().as_string());
  Deserialize<uint8_t>::to(destination.HealthStatus, doc.select_node("INSTANCE/PROPERTY[@NAME=\"HealthStatus\"]/VALUE").node().text().as_string());
  Deserialize<bool>::to(destination.Redirected, doc.select_node("INSTANCE/PROPERTY[@NAME=\"Redirected\"]/VALUE").node().text().as_string());
  Deserialize<uint8_t>::to(destination.LastSyncStatus, doc.select_node("INSTANCE/PROPERTY[@NAME=\"LastSyncStatus\"]/VALUE").node().text().as_string());
  Deserialize<bool>::to(destination.OfflineAccessEnabled, doc.select_node("INSTANCE/PROPERTY[@NAME=\"OfflineAccessEnabled\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.LastSyncTime, doc.select_node("INSTANCE/PROPERTY[@NAME=\"LastSyncTime\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.OfflineFileNameFolderGUID, doc.select_node("INSTANCE/PROPERTY[@NAME=\"OfflineFileNameFolderGUID\"]/VALUE").node().text().as_string());
}

std::vector<Win32_FolderRedirectionHealth> Win32_FolderRedirectionHealth::get_all_objects()
{
  std::vector<Win32_FolderRedirectionHealth> result;
  WMIProvider::get().query("select * from Win32_FolderRedirectionHealth", [&](IWbemClassObject* o, const WmiConnection&, const pugi::xml_document& doc) {
    Win32_FolderRedirectionHealth cpp_obj;
    Win32_FolderRedirectionHealth::deserialize(doc, cpp_obj);
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
void Win32_UserProfile::deserialize(const pugi::xml_document& doc, Win32_UserProfile& destination)
{
  Deserialize<uint32_t>::to(destination.RefCount, doc.select_node("INSTANCE/PROPERTY[@NAME=\"RefCount\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.LastUseTime, doc.select_node("INSTANCE/PROPERTY[@NAME=\"LastUseTime\"]/VALUE").node().text().as_string());
  Deserialize<uint8_t>::to(destination.HealthStatus, doc.select_node("INSTANCE/PROPERTY[@NAME=\"HealthStatus\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.LastUploadTime, doc.select_node("INSTANCE/PROPERTY[@NAME=\"LastUploadTime\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.LastAttemptedProfileDownloadTime, doc.select_node("INSTANCE/PROPERTY[@NAME=\"LastAttemptedProfileDownloadTime\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.LastDownloadTime, doc.select_node("INSTANCE/PROPERTY[@NAME=\"LastDownloadTime\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.LastAttemptedProfileUploadTime, doc.select_node("INSTANCE/PROPERTY[@NAME=\"LastAttemptedProfileUploadTime\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.LastBackgroundRegistryUploadTime, doc.select_node("INSTANCE/PROPERTY[@NAME=\"LastBackgroundRegistryUploadTime\"]/VALUE").node().text().as_string());
  Deserialize<bool>::to(destination.Loaded, doc.select_node("INSTANCE/PROPERTY[@NAME=\"Loaded\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.LocalPath, doc.select_node("INSTANCE/PROPERTY[@NAME=\"LocalPath\"]/VALUE").node().text().as_string());
  Deserialize<bool>::to(destination.RoamingConfigured, doc.select_node("INSTANCE/PROPERTY[@NAME=\"RoamingConfigured\"]/VALUE").node().text().as_string());
  Deserialize<uint32_t>::to(destination.Status, doc.select_node("INSTANCE/PROPERTY[@NAME=\"Status\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.SID, doc.select_node("INSTANCE/PROPERTY[@NAME=\"SID\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.RoamingPath, doc.select_node("INSTANCE/PROPERTY[@NAME=\"RoamingPath\"]/VALUE").node().text().as_string());
  Deserialize<bool>::to(destination.RoamingPreference, doc.select_node("INSTANCE/PROPERTY[@NAME=\"RoamingPreference\"]/VALUE").node().text().as_string());
  Deserialize<bool>::to(destination.Special, doc.select_node("INSTANCE/PROPERTY[@NAME=\"Special\"]/VALUE").node().text().as_string());

  const auto docs = doc.select_node("INSTANCE/PROPERTY.OBJECT[@NAME=\"Documents\"]/INSTANCE");
  Win32_FolderRedirectionHealth::deserialize(docs, destination.Documents);
  
}

std::vector<Win32_UserProfile> Win32_UserProfile::get_all_objects()
{
  std::vector<Win32_UserProfile> result;
  WMIProvider::get().query("select * from Win32_UserProfile", [&](IWbemClassObject* o, const WmiConnection&, const pugi::xml_document& doc) {
    Win32_UserProfile cpp_obj;
    Win32_UserProfile::deserialize(doc, cpp_obj);

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
