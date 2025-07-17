// PowerInformation.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// AC: Value or behavior when plugged into external power (wall outlet/dock). Typically allows higher performance and longer display time.
// DC: Value or behavior when running on battery. Typically uses conservative settings to save energy (dimming display, reducing CPU performance, etc).
//
// Main program:
//   - Dumps processor core type info (Intel Hybrid arch).
//   - Supports command-line Get/Set for power settings.
//   - Dumps filtered power settings if no arguments are provided.
//
// Usage:
//   PowerInformation.exe Help
//     - Prints usage instructions and sample commands.
//   PowerInformation.exe Get "<profile name>" "<setting name>"
//     - Prints AC/DC values for the specified setting in the specified profile.
//   PowerInformation.exe Set "<profile name>" "<setting name>" <value>
//     - Sets AC/DC values for the specified setting in the specified profile.
//
// Example:
//   PowerInformation.exe Get "Balanced" "Heterogeneous thread scheduling policy"
//     // Output: AC value: <value>\nDC value: <value>
//   PowerInformation.exe Set "Balanced" "Heterogeneous thread scheduling policy" 1
//     // Sets both AC and DC values to 1 for the specified setting.
//
//   // AC and DC usage:
//   // - AC refers to the value when plugged in (wall outlet/dock).
//   // - DC refers to the value when running on battery.
//   // The program always gets/sets both AC and DC values for the specified setting.
//
#include "pch.h"
#include "PInformation.h"
#include "PProcInformation.h"
#include <iostream>
#include <Windows.h>
#include <algorithm>

// Checks if a setting name matches thread scheduling policies
bool containsThreadType(const std::wstring& text) {
	std::wstring lowerText = text;
	std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::towlower);
	return lowerText.find(L"heterogeneous thread scheduling policy") != std::wstring::npos ||
		   lowerText.find(L"heterogeneous short running thread scheduling policy") != std::wstring::npos;
}

// Entry point
int wmain(int argc, wchar_t* argv[])
{
	PInformation pInfo;

	// Help parameter support
	if (argc >= 2 && (wcscmp(argv[1], L"Help") == 0 || wcscmp(argv[1], L"--help") == 0 || wcscmp(argv[1], L"-h") == 0)) {
		std::wcout << L"Usage:\n"
			<< L"  PowerInformation.exe Help\n"
			<< L"    - Prints usage instructions and sample commands.\n"
			<< L"  PowerInformation.exe Get \"<profile name>\" \"<setting name>\"\n"
			<< L"    - Prints AC/DC values for the specified setting in the specified profile.\n"
			<< L"  PowerInformation.exe Set \"<profile name>\" \"<setting name>\" <value>\n"
			<< L"    - Sets AC/DC values for the specified setting in the specified profile.\n"
			<< L"\nExample:\n"
			<< L"  PowerInformation.exe Get \"Balanced\" \"Heterogeneous thread scheduling policy\"\n"
			<< L"  PowerInformation.exe Set \"Balanced\" \"Heterogeneous thread scheduling policy\" 1\n"
			<< L"\nAC refers to plugged-in power, DC refers to battery. Both are always shown/set.\n";
		return 0;
	}

	// Command-line Get/Set support
	if (argc >= 2)
	{
		std::wstring command = argv[1];
		if (command == L"Get" && argc >= 4)
		{
			std::wstring profile = argv[2];
			std::wstring setting = argv[3];
			DWORD value = 0;
			if (pInfo.GetPowerSettingValue(profile, setting, true, value))
				std::wcout << L"AC value: " << value << std::endl;
			else
				std::wcout << L"Failed to get AC value." << std::endl;
			if (pInfo.GetPowerSettingValue(profile, setting, false, value))
				std::wcout << L"DC value: " << value << std::endl;
			else
				std::wcout << L"Failed to get DC value." << std::endl;
			return 0;
		}
		else if (command == L"Set" && argc >= 5)
		{
			std::wstring profile = argv[2];
			std::wstring setting = argv[3];
			DWORD value = _wtoi(argv[4]);
			bool okAC = pInfo.SetPowerSettingValue(profile, setting, value, true);
			bool okDC = pInfo.SetPowerSettingValue(profile, setting, value, false);
			if (okAC || okDC)
				std::wcout << L"Set value successfully." << std::endl;
			else
				std::wcout << L"Failed to set value." << std::endl;
			return 0;
		}
	}

	// Default: dump processor info and filtered settings
	PProcInformation procInfo;
	procInfo.DumpCoreTypes();

	auto defaultprofile = pInfo.GetDefaultPowerProfileName();
	std::wcout << L"Default Power Profile: " << defaultprofile << std::endl;
	std::map<std::wstring, std::vector<SettingInfo>> profiles = pInfo.PowerEnumerateProfiles();
	std::wcout << L"Available Power Profiles and Filtered Settings:\n";
	for (const auto& profile : profiles)
	{
		for (const auto& setting : profile.second)
		{
			if (containsThreadType(setting.name)) {
				std::wcout << L"    Setting: " << setting.name << L" - " << setting.description
						   << L", AC: " << setting.acValue << L", DC: " << setting.dcValue << std::endl;
			}
		}
	}
	return 0;
}
