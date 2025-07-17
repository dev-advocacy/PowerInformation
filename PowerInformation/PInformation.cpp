#include "pch.h"
#include "PInformation.h"
#include <wil/resource.h>

PInformation::PInformation()
{
}

PInformation::~PInformation()
{
}

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

	DWORD acMax = 0, acMin = 0, dcMax = 0, dcMin = 0;

	if (PowerReadACValueIndex(NULL, pPwrGUID.get(), &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROCESSOR_THROTTLE_MAXIMUM, &acMax) ||
		PowerReadACValueIndex(NULL, pPwrGUID.get(), &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROCESSOR_THROTTLE_MINIMUM, &acMin) ||
		PowerReadDCValueIndex(NULL, pPwrGUID.get(), &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROCESSOR_THROTTLE_MAXIMUM, &dcMax) ||
		PowerReadDCValueIndex(NULL, pPwrGUID.get(), &GUID_PROCESSOR_SETTINGS_SUBGROUP, &GUID_PROCESSOR_THROTTLE_MINIMUM, &dcMin))
		return std::wstring();

	return friendlyName;
}

void PInformation::PowerEnumerateProfiles()
{
	int   scheme_idx = 0;
	GUID  scheme_guid = { };
	DWORD guid_size = sizeof(GUID);

	while (ERROR_SUCCESS == PowerEnumerate(nullptr, nullptr, nullptr, ACCESS_SCHEME, scheme_idx++, (UCHAR*)&scheme_guid, &guid_size))
	{
		power_scheme_s scheme;
		scheme.uid = scheme_guid;
		resolveNameAndDescForPowerScheme(scheme);
		scheme_guid = { };
		guid_size = sizeof(GUID);
	}
}

void PInformation::resolveNameAndDescForPowerScheme(power_scheme_s& scheme)
{
	DWORD   dwLen = 511;
	wchar_t wszName[512] = { };

	DWORD dwRet = PowerReadFriendlyName(nullptr,
										&scheme.uid,
										nullptr, nullptr,
										(PUCHAR)wszName,
										&dwLen);

	if (dwRet == ERROR_MORE_DATA)
	{
		return; // Handle the case where the buffer is too small
	}
	else if (dwRet == ERROR_SUCCESS)
	{
		wchar_t wszDesc[512] = { };
		dwLen = 511;
		dwRet = PowerReadDescription(nullptr,
									 &scheme.uid,
									 nullptr, nullptr,
									 (PUCHAR)wszDesc,
									 &dwLen);
		if (dwRet == ERROR_MORE_DATA)
		{
			return; // Handle the case where the buffer is too small	
		}
		else if (dwRet == ERROR_SUCCESS)
		{
			ATLTRACE(L"Power Scheme Name: %s , Description : %s \n ", wszName, wszDesc);
		}
	}
}