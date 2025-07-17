// PInformation.cpp - Implements power profile and setting enumeration, retrieval, and modification.
//
// This file provides:
// - Helper functions for string conversion and power setting name/description retrieval.
// - PInformation class methods for enumerating power profiles and settings.
// - Methods to get and set power setting values for specific profiles/settings.
//
// Usage:
//   - Enumerate all profiles and their settings.
//   - Get or set a specific setting value for a profile//
//
#include "pch.h"
#include "PInformation.h"

// Helper function to convert wide string to UTF-8 std::string
std::string WideToUtf8(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Helper function to convert wide C-string to UTF-8 std::string
std::string WideToUtf8(const wchar_t* wstr)
{
    if (!wstr) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, (int)wcslen(wstr), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr, (int)wcslen(wstr), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Helper function to read friendly name for a power setting
static std::wstring ReadFriendlyName(const GUID* scheme, const GUID* subgroup, const GUID* setting)
{
    wchar_t buffer[512] = {};
    DWORD bufSize = sizeof(buffer);
    DWORD ret = PowerReadFriendlyName(NULL, scheme, subgroup, setting, (PUCHAR)buffer, &bufSize);
    if (ret == ERROR_SUCCESS) return buffer;
    return L"";
}

// Helper function to read description for a power setting
static std::wstring ReadDescription(const GUID* scheme, const GUID* subgroup, const GUID* setting)
{
    wchar_t buffer[512] = {};
    DWORD bufSize = sizeof(buffer);
    DWORD ret = PowerReadDescription(NULL, scheme, subgroup, setting, (PUCHAR)buffer, &bufSize);
    if (ret == ERROR_SUCCESS) return buffer;
    return L"";
}

// Constructor
PInformation::PInformation() {}

// Destructor
PInformation::~PInformation() {}

// Get the friendly name of the currently active power profile
std::wstring PInformation::GetDefaultPowerProfileName()
{
    USES_CONVERSION;
    wil::unique_any<GUID*, decltype(&::LocalFree), ::LocalFree> pPwrGUID;
    DWORD ret = PowerGetActiveScheme(NULL, pPwrGUID.put());
    if (ret != ERROR_SUCCESS)
        return std::wstring();

    UCHAR aBuffer[2048];
    DWORD aBufferSize = sizeof(aBuffer);
    ret = PowerReadFriendlyName(NULL, pPwrGUID.get(), &NO_SUBGROUP_GUID, NULL, aBuffer, &aBufferSize);
    std::wstring friendlyName = (wchar_t*)aBuffer;
    if (ret != ERROR_SUCCESS)
        return std::wstring();
    return friendlyName;
}

// Enumerate all settings and their AC/DC values for a given power scheme
std::vector<SettingInfo> PInformation::EnumerateAllSettingsValues(const GUID* schemeGuid)
{
    std::vector<SettingInfo> settingsList;
    if (!schemeGuid) return settingsList;
    DWORD subgroup_idx = 0;
    GUID subgroup_guid = {};
    DWORD guid_size = sizeof(GUID);
    while (ERROR_SUCCESS == PowerEnumerate(NULL, schemeGuid, NULL, ACCESS_SUBGROUP, subgroup_idx++, (UCHAR*)&subgroup_guid, &guid_size)) {
        DWORD setting_idx = 0;
        GUID setting_guid = {};
        DWORD setting_guid_size = sizeof(GUID);
        while (ERROR_SUCCESS == PowerEnumerate(NULL, schemeGuid, &subgroup_guid, ACCESS_INDIVIDUAL_SETTING, setting_idx++, (UCHAR*)&setting_guid, &setting_guid_size)) {
            SettingInfo info;
            info.name = ReadFriendlyName(schemeGuid, &subgroup_guid, &setting_guid);
            info.description = ReadDescription(schemeGuid, &subgroup_guid, &setting_guid);
            if (info.name.empty()) {
                wchar_t setting_guid_str[64] = {};
                StringFromGUID2(setting_guid, setting_guid_str, 64);
                info.name = setting_guid_str;
            }
            DWORD type = 0;
            BYTE buffer[256] = {};
            DWORD bufferSize = sizeof(buffer);
            DWORD ret = PowerReadACValue(NULL, schemeGuid, &subgroup_guid, &setting_guid, &type, buffer, &bufferSize);
            info.acValue = (ret == ERROR_SUCCESS) ? std::to_wstring(*(DWORD*)buffer) : L"<error>";
            bufferSize = sizeof(buffer);
            ret = PowerReadDCValue(NULL, schemeGuid, &subgroup_guid, &setting_guid, &type, buffer, &bufferSize);
            info.dcValue = (ret == ERROR_SUCCESS) ? std::to_wstring(*(DWORD*)buffer) : L"<error>";
            settingsList.push_back(info);
        }
    }
    return settingsList;
}

// Enumerate all power profiles and their settings
std::map<std::wstring, std::vector<SettingInfo>> PInformation::PowerEnumerateProfiles()
{
    std::map<std::wstring, std::vector<SettingInfo>> profileSettingsMap;
    DWORD scheme_idx = 0;
    while (true)
    {
        GUID scheme_guid = {};
        DWORD guid_size = sizeof(GUID);
        DWORD status = PowerEnumerate(nullptr, nullptr, nullptr, ACCESS_SCHEME, scheme_idx, (UCHAR*)&scheme_guid, &guid_size);
        if (status == ERROR_NO_MORE_ITEMS)
            break;
        if (status != ERROR_SUCCESS)
        {
            scheme_idx++;
            continue;
        }
        wchar_t wszName[512] = {};
        DWORD dwLen = 511;
        DWORD dwRet = PowerReadFriendlyName(nullptr, &scheme_guid, nullptr, nullptr, (PUCHAR)wszName, &dwLen);
        std::wstring profileName = (dwRet == ERROR_SUCCESS) ? wszName : L"";
        if (profileName.empty()) {
            wchar_t scheme_guid_str[64] = {};
            StringFromGUID2(scheme_guid, scheme_guid_str, 64);
            profileName = scheme_guid_str;
        }
        std::vector<SettingInfo> settings = EnumerateAllSettingsValues(&scheme_guid);
        profileSettingsMap[profileName] = settings;
        scheme_idx++;
    }
    return profileSettingsMap;
}

// Helper to resolve name and description for a power scheme
void PInformation::resolveNameAndDescForPowerScheme(power_scheme_s& scheme, std::map<std::wstring, SettingInfo>& powerProfiles)
{
    DWORD dwLen = 511;
    wchar_t wszName[512] = {};
    DWORD dwRet = PowerReadFriendlyName(nullptr,
                                        &scheme.uid,
                                        nullptr, nullptr,
                                        (PUCHAR)wszName,
                                        &dwLen);
    if (dwRet == ERROR_MORE_DATA)
    {
        return;
    }
    else if (dwRet == ERROR_SUCCESS)
    {
        wchar_t wszDesc[512] = {};
        dwLen = 511;
        dwRet = PowerReadDescription(nullptr,
                                     &scheme.uid,
                                     nullptr, nullptr,
                                     (PUCHAR)wszDesc,
                                     &dwLen);
        if (dwRet == ERROR_MORE_DATA)
        {
            return;
        }
        else if (dwRet == ERROR_SUCCESS)
        {
            SettingInfo info;
            info.name = wszName;
            info.description = wszDesc;
            powerProfiles[wszName] = info;
        }
    }
}

// Set a power setting value for a specific profile and setting
bool PInformation::SetPowerSettingValue(const std::wstring& profileName, const std::wstring& settingName, DWORD value, bool ac)
{
    // Find profile GUID
    int scheme_idx = 0;
    GUID scheme_guid = {};
    DWORD guid_size = sizeof(GUID);
    while (ERROR_SUCCESS == PowerEnumerate(nullptr, nullptr, nullptr, ACCESS_SCHEME, scheme_idx++, (UCHAR*)&scheme_guid, &guid_size)) {
        wchar_t wszName[512] = {};
        DWORD dwLen = 511;
        DWORD dwRet = PowerReadFriendlyName(nullptr, &scheme_guid, nullptr, nullptr, (PUCHAR)wszName, &dwLen);
        std::wstring foundProfile = (dwRet == ERROR_SUCCESS) ? wszName : L"";
        if (foundProfile.empty()) {
            wchar_t scheme_guid_str[64] = {};
            StringFromGUID2(scheme_guid, scheme_guid_str, 64);
            foundProfile = scheme_guid_str;
        }
        if (foundProfile == profileName) {
            // Find setting GUID
            DWORD subgroup_idx = 0;
            GUID subgroup_guid = {};
            DWORD subgroup_guid_size = sizeof(GUID);
            while (ERROR_SUCCESS == PowerEnumerate(nullptr, &scheme_guid, nullptr, ACCESS_SUBGROUP, subgroup_idx++, (UCHAR*)&subgroup_guid, &subgroup_guid_size)) {
                DWORD setting_idx = 0;
                GUID setting_guid = {};
                DWORD setting_guid_size = sizeof(GUID);
                while (ERROR_SUCCESS == PowerEnumerate(nullptr, &scheme_guid, &subgroup_guid, ACCESS_INDIVIDUAL_SETTING, setting_idx++, (UCHAR*)&setting_guid, &setting_guid_size)) {
                    std::wstring foundSetting = ReadFriendlyName(&scheme_guid, &subgroup_guid, &setting_guid);
                    if (foundSetting.empty()) {
                        wchar_t setting_guid_str[64] = {};
                        StringFromGUID2(setting_guid, setting_guid_str, 64);
                        foundSetting = setting_guid_str;
                    }
                    if (foundSetting == settingName) {
                        // Set value for AC or DC
                        DWORD ret;
                        if (ac)
                            ret = PowerWriteACValueIndex(nullptr, &scheme_guid, &subgroup_guid, &setting_guid, value);
                        else
                            ret = PowerWriteDCValueIndex(nullptr, &scheme_guid, &subgroup_guid, &setting_guid, value);
                        PowerSetActiveScheme(nullptr, &scheme_guid);
                        return ret == ERROR_SUCCESS;
                    }
                }
            }
        }
        scheme_guid = {};
        guid_size = sizeof(GUID);
    }
    return false;
}

// Get a power setting value for a specific profile and setting
bool PInformation::GetPowerSettingValue(const std::wstring& profileName, const std::wstring& settingName, bool ac, DWORD& outValue)
{
    int scheme_idx = 0;
    GUID scheme_guid = {};
    DWORD guid_size = sizeof(GUID);
    while (ERROR_SUCCESS == PowerEnumerate(nullptr, nullptr, nullptr, ACCESS_SCHEME, scheme_idx++, (UCHAR*)&scheme_guid, &guid_size)) {
        wchar_t wszName[512] = {};
        DWORD dwLen = 511;
        DWORD dwRet = PowerReadFriendlyName(nullptr, &scheme_guid, nullptr, nullptr, (PUCHAR)wszName, &dwLen);
        std::wstring foundProfile = (dwRet == ERROR_SUCCESS) ? wszName : L"";
        if (foundProfile.empty()) {
            wchar_t scheme_guid_str[64] = {};
            StringFromGUID2(scheme_guid, scheme_guid_str, 64);
            foundProfile = scheme_guid_str;
        }
        if (foundProfile == profileName) {
            DWORD subgroup_idx = 0;
            GUID subgroup_guid = {};
            DWORD subgroup_guid_size = sizeof(GUID);
            while (ERROR_SUCCESS == PowerEnumerate(nullptr, &scheme_guid, nullptr, ACCESS_SUBGROUP, subgroup_idx++, (UCHAR*)&subgroup_guid, &subgroup_guid_size)) {
                DWORD setting_idx = 0;
                GUID setting_guid = {};
                DWORD setting_guid_size = sizeof(GUID);
                while (ERROR_SUCCESS == PowerEnumerate(nullptr, &scheme_guid, &subgroup_guid, ACCESS_INDIVIDUAL_SETTING, setting_idx++, (UCHAR*)&setting_guid, &setting_guid_size)) {
                    std::wstring foundSetting = ReadFriendlyName(&scheme_guid, &subgroup_guid, &setting_guid);
                    if (foundSetting.empty()) {
                        wchar_t setting_guid_str[64] = {};
                        StringFromGUID2(setting_guid, setting_guid_str, 64);
                        foundSetting = setting_guid_str;
                    }
                    if (foundSetting == settingName) {
                        DWORD type = 0;
                        BYTE buffer[256] = {};
                        DWORD bufferSize = sizeof(buffer);
                        DWORD ret;
                        if (ac)
                            ret = PowerReadACValue(nullptr, &scheme_guid, &subgroup_guid, &setting_guid, &type, buffer, &bufferSize);
                        else
                            ret = PowerReadDCValue(nullptr, &scheme_guid, &subgroup_guid, &setting_guid, &type, buffer, &bufferSize);
                        if (ret == ERROR_SUCCESS) {
                            outValue = *(DWORD*)buffer;
                            return true;
                        }
                        return false;
                    }
                }
            }
        }
        scheme_guid = {};
        guid_size = sizeof(GUID);
    }
    return false;
}