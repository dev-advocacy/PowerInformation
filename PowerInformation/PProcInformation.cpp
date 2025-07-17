// PProcInformation.cpp - Implements processor core type detection for Intel Hybrid architecture.
//
// This file provides:
// - Detection of P-core and E-core counts using Windows API.
// - Console output of detected core types.
//
#include "pch.h"
#include "PProcInformation.h"
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

// Constructor: Detects core types on initialization
PProcInformation::PProcInformation() {
    DetectCoreTypes();
}

// Destructor
PProcInformation::~PProcInformation() {}

// Returns true if Intel Hybrid architecture is detected
bool PProcInformation::IsIntelHybridArchDetected() {
    return intelHybridArchDetected;
}

// Dumps P-core and E-core counts to console
void PProcInformation::DumpCoreTypes() {
    std::cout << "Intel Hybrid Architecture Detected: " << (intelHybridArchDetected ? "Yes" : "No") << std::endl;
    std::cout << "P-Cores: " << pCoreCount << std::endl;
    std::cout << "E-Cores: " << eCoreCount << std::endl;
}

// Detects core types using Windows API (Windows 11+)
void PProcInformation::DetectCoreTypes() {
#ifdef _WIN32
    DWORD len = 0;
    GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &len);
    std::vector<BYTE> buffer(len);
    if (!GetLogicalProcessorInformationEx(RelationProcessorCore, reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data()), &len)) return;

    BYTE* ptr = buffer.data();
    BYTE* end = buffer.data() + len;
    while (ptr < end) {
        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX coreInfo = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(ptr);
        if (coreInfo->Relationship == RelationProcessorCore) {
            // EfficiencyClass: 0 = P-core, 1 = E-core (Windows 11+)
            if (coreInfo->Processor.EfficiencyClass == 0)
                pCoreCount++;
            else if (coreInfo->Processor.EfficiencyClass == 1)
                eCoreCount++;
        }
        ptr += coreInfo->Size;
    }
    intelHybridArchDetected = (pCoreCount > 0 && eCoreCount > 0);
#endif
}
