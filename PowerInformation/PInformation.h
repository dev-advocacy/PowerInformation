#pragma once



struct power_scheme_s {
    GUID uid;
    char utf8_name[512];
    char utf8_desc[512];
};


class PInformation
{
public:
    PInformation();
    ~PInformation();

    std::wstring GetDefaultPowerProfileName();
    void PowerEnumerateProfiles();
private:
    void resolveNameAndDescForPowerScheme(power_scheme_s& scheme);
};

