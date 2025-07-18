// PInformation.h - Declares types and the PInformation class for power profile/setting management.
//
// Types:
//   - power_scheme_s: Holds GUID and name/description for a power scheme.
//   - SettingInfo: Holds name, description, AC/DC values for a power setting.
//
// PInformation class:
//   - Enumerates power profiles and settings.
//   - Gets/sets power setting values for specific profiles/settings.
//
#pragma once
#include <vector>
#include <map>
#include <string>

// Structure for power scheme information
struct power_scheme_s {
    GUID uid;
    char utf8_name[512];
    char utf8_desc[512];
};

// Structure for power setting information
struct SettingInfo {
    std::wstring name;
    std::wstring description;
    std::wstring acValue;
    std::wstring dcValue;
};

// Main class for power profile/setting management
class PInformation
{
public:
    PInformation();
    ~PInformation();

    std::wstring GetDefaultPowerProfileName();
    std::map<std::wstring, std::vector<SettingInfo>> PowerEnumerateProfiles(); // profile name -> settings
    std::vector<SettingInfo> EnumerateAllSettingsValues(const GUID* schemeGuid); // settings for a given profile
    void resolveNameAndDescForPowerScheme(power_scheme_s& scheme, std::map<std::wstring, SettingInfo>& powerProfiles);

    // Set a power setting value for a specific profile/setting
    bool SetPowerSettingValue(const std::wstring& profileName, const std::wstring& settingName, DWORD value, bool ac); // ac=true for AC, false for DC
    // Get a power setting value for a specific profile/setting
    bool GetPowerSettingValue(const std::wstring& profileName, const std::wstring& settingName, bool ac, DWORD& outValue);
};


