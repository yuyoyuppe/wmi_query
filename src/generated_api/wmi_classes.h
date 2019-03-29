#pragma once
#include <string>
#include <vector>
#include <chrono>
struct Win32_FolderRedirectionHealth
{
  std::string LastSuccessfulSyncTime;
  uint8_t HealthStatus;
  bool Redirected;
  uint8_t LastSyncStatus;
  bool OfflineAccessEnabled;
  std::string LastSyncTime;
  std::string OfflineFileNameFolderGUID;

  static std::vector<Win32_FolderRedirectionHealth> get_all_objects();
  std::string to_string() const;
};

struct Win32_UserProfile
{
  uint32_t RefCount;
  std::string LastUseTime;
  uint8_t HealthStatus;
  std::string LastUploadTime;
  std::string LastAttemptedProfileDownloadTime;
  std::string LastDownloadTime;
  std::string LastAttemptedProfileUploadTime;
  std::string LastBackgroundRegistryUploadTime;
  bool Loaded;
  std::string LocalPath;
  bool RoamingConfigured;
  uint32_t Status;
  std::string SID;
  std::string RoamingPath;
  bool RoamingPreference;
  bool Special;

// object!
  Win32_FolderRedirectionHealth AppDataRoaming;
  Win32_FolderRedirectionHealth Desktop;
  Win32_FolderRedirectionHealth StartMenu;
  Win32_FolderRedirectionHealth Contacts;
  Win32_FolderRedirectionHealth Documents;
  Win32_FolderRedirectionHealth Pictures;
  Win32_FolderRedirectionHealth Links;
  Win32_FolderRedirectionHealth Downloads;
  Win32_FolderRedirectionHealth Favorites;
  Win32_FolderRedirectionHealth Music;
  Win32_FolderRedirectionHealth SavedGames;
  Win32_FolderRedirectionHealth Searches;
  Win32_FolderRedirectionHealth Videos;

  static std::vector<Win32_UserProfile> get_all_objects();
  std::string to_string() const;
};

