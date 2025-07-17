// PowerInformation.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include "PInformation.h"
int main()
{
	PInformation pInfo;
	


	pInfo.PowerEnumerateProfiles();

	std::wstring powerProfileName = pInfo.GetDefaultPowerProfileName();

	if (!powerProfileName.empty())
	{
		ATLTRACE(L"Active Power Profile: %s\n", powerProfileName.c_str());
	}
	else
	{
		ATLTRACE(L"Failed to retrieve power profile name.\n");
	}
	return 0;
}



