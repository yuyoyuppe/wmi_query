
#include "pch.h"
#include "wmi_classes.h"

namespace wmi{
void Win32_FolderRedirectionHealth::deserialize(const pugi::xml_node& doc, Win32_FolderRedirectionHealth& destination)
{
  Deserialize<std::string>::to(destination.LastSuccessfulSyncTime, doc.select_node("PROPERTY[@NAME=\"LastSuccessfulSyncTime\"]/VALUE").node().text().as_string());
  Deserialize<uint8_t>::to(destination.HealthStatus, doc.select_node("PROPERTY[@NAME=\"HealthStatus\"]/VALUE").node().text().as_string());
  Deserialize<bool>::to(destination.Redirected, doc.select_node("PROPERTY[@NAME=\"Redirected\"]/VALUE").node().text().as_string());
  Deserialize<uint8_t>::to(destination.LastSyncStatus, doc.select_node("PROPERTY[@NAME=\"LastSyncStatus\"]/VALUE").node().text().as_string());
  Deserialize<bool>::to(destination.OfflineAccessEnabled, doc.select_node("PROPERTY[@NAME=\"OfflineAccessEnabled\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.LastSyncTime, doc.select_node("PROPERTY[@NAME=\"LastSyncTime\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.OfflineFileNameFolderGUID, doc.select_node("PROPERTY[@NAME=\"OfflineFileNameFolderGUID\"]/VALUE").node().text().as_string());
}

std::vector<Win32_FolderRedirectionHealth> Win32_FolderRedirectionHealth::get_all_objects()
{
  std::vector<Win32_FolderRedirectionHealth> result;
  WMIProvider::get().query("select * from Win32_FolderRedirectionHealth", [&](IWbemClassObject* o, const WmiConnection&, const pugi::xml_document& doc) {
    Win32_FolderRedirectionHealth cpp_obj;
    Win32_FolderRedirectionHealth::deserialize(doc.child("INSTANCE"), cpp_obj);
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
void Win32_UserProfile::deserialize(const pugi::xml_node& doc, Win32_UserProfile& destination)
{
  Deserialize<uint32_t>::to(destination.RefCount, doc.select_node("PROPERTY[@NAME=\"RefCount\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.LastUseTime, doc.select_node("PROPERTY[@NAME=\"LastUseTime\"]/VALUE").node().text().as_string());
  Deserialize<uint8_t>::to(destination.HealthStatus, doc.select_node("PROPERTY[@NAME=\"HealthStatus\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.LastUploadTime, doc.select_node("PROPERTY[@NAME=\"LastUploadTime\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.LastAttemptedProfileDownloadTime, doc.select_node("PROPERTY[@NAME=\"LastAttemptedProfileDownloadTime\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.LastDownloadTime, doc.select_node("PROPERTY[@NAME=\"LastDownloadTime\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.LastAttemptedProfileUploadTime, doc.select_node("PROPERTY[@NAME=\"LastAttemptedProfileUploadTime\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.LastBackgroundRegistryUploadTime, doc.select_node("PROPERTY[@NAME=\"LastBackgroundRegistryUploadTime\"]/VALUE").node().text().as_string());
  Deserialize<bool>::to(destination.Loaded, doc.select_node("PROPERTY[@NAME=\"Loaded\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.LocalPath, doc.select_node("PROPERTY[@NAME=\"LocalPath\"]/VALUE").node().text().as_string());
  Deserialize<bool>::to(destination.RoamingConfigured, doc.select_node("PROPERTY[@NAME=\"RoamingConfigured\"]/VALUE").node().text().as_string());
  Deserialize<uint32_t>::to(destination.Status, doc.select_node("PROPERTY[@NAME=\"Status\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.SID, doc.select_node("PROPERTY[@NAME=\"SID\"]/VALUE").node().text().as_string());
  Deserialize<std::string>::to(destination.RoamingPath, doc.select_node("PROPERTY[@NAME=\"RoamingPath\"]/VALUE").node().text().as_string());
  Deserialize<bool>::to(destination.RoamingPreference, doc.select_node("PROPERTY[@NAME=\"RoamingPreference\"]/VALUE").node().text().as_string());
  Deserialize<bool>::to(destination.Special, doc.select_node("PROPERTY[@NAME=\"Special\"]/VALUE").node().text().as_string());
  Win32_FolderRedirectionHealth::deserialize(doc.select_node("INSTANCE/PROPERTY.OBJECT[@NAME=\"AppDataRoaming\"]/INSTANCE").node(), destination.AppDataRoaming);
  Win32_FolderRedirectionHealth::deserialize(doc.select_node("INSTANCE/PROPERTY.OBJECT[@NAME=\"Desktop\"]/INSTANCE").node(), destination.Desktop);
  Win32_FolderRedirectionHealth::deserialize(doc.select_node("INSTANCE/PROPERTY.OBJECT[@NAME=\"StartMenu\"]/INSTANCE").node(), destination.StartMenu);
  Win32_FolderRedirectionHealth::deserialize(doc.select_node("INSTANCE/PROPERTY.OBJECT[@NAME=\"Contacts\"]/INSTANCE").node(), destination.Contacts);
  Win32_FolderRedirectionHealth::deserialize(doc.select_node("INSTANCE/PROPERTY.OBJECT[@NAME=\"Documents\"]/INSTANCE").node(), destination.Documents);
  Win32_FolderRedirectionHealth::deserialize(doc.select_node("INSTANCE/PROPERTY.OBJECT[@NAME=\"Pictures\"]/INSTANCE").node(), destination.Pictures);
  Win32_FolderRedirectionHealth::deserialize(doc.select_node("INSTANCE/PROPERTY.OBJECT[@NAME=\"Links\"]/INSTANCE").node(), destination.Links);
  Win32_FolderRedirectionHealth::deserialize(doc.select_node("INSTANCE/PROPERTY.OBJECT[@NAME=\"Downloads\"]/INSTANCE").node(), destination.Downloads);
  Win32_FolderRedirectionHealth::deserialize(doc.select_node("INSTANCE/PROPERTY.OBJECT[@NAME=\"Favorites\"]/INSTANCE").node(), destination.Favorites);
  Win32_FolderRedirectionHealth::deserialize(doc.select_node("INSTANCE/PROPERTY.OBJECT[@NAME=\"Music\"]/INSTANCE").node(), destination.Music);
  Win32_FolderRedirectionHealth::deserialize(doc.select_node("INSTANCE/PROPERTY.OBJECT[@NAME=\"SavedGames\"]/INSTANCE").node(), destination.SavedGames);
  Win32_FolderRedirectionHealth::deserialize(doc.select_node("INSTANCE/PROPERTY.OBJECT[@NAME=\"Searches\"]/INSTANCE").node(), destination.Searches);
  Win32_FolderRedirectionHealth::deserialize(doc.select_node("INSTANCE/PROPERTY.OBJECT[@NAME=\"Videos\"]/INSTANCE").node(), destination.Videos);
}

std::vector<Win32_UserProfile> Win32_UserProfile::get_all_objects()
{
  std::vector<Win32_UserProfile> result;
  WMIProvider::get().query("select * from Win32_UserProfile", [&](IWbemClassObject* o, const WmiConnection&, const pugi::xml_document& doc) {
    Win32_UserProfile cpp_obj;
    Win32_UserProfile::deserialize(doc.child("INSTANCE"), cpp_obj);
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
  oss << "AppDataRoaming: " << AppDataRoaming.to_string() << std::endl;
  oss << "Desktop: " << Desktop.to_string() << std::endl;
  oss << "StartMenu: " << StartMenu.to_string() << std::endl;
  oss << "Contacts: " << Contacts.to_string() << std::endl;
  oss << "Documents: " << Documents.to_string() << std::endl;
  oss << "Pictures: " << Pictures.to_string() << std::endl;
  oss << "Links: " << Links.to_string() << std::endl;
  oss << "Downloads: " << Downloads.to_string() << std::endl;
  oss << "Favorites: " << Favorites.to_string() << std::endl;
  oss << "Music: " << Music.to_string() << std::endl;
  oss << "SavedGames: " << SavedGames.to_string() << std::endl;
  oss << "Searches: " << Searches.to_string() << std::endl;
  oss << "Videos: " << Videos.to_string() << std::endl;
  return oss.str();
}

} //namespace wmi
