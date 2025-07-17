// PProcInformation.h - Declares PProcInformation for processor core type detection.
//
// PProcInformation class:
//   - Detects Intel Hybrid architecture (P-core/E-core).
 //   - Dumps core type counts.
//
#pragma once
#include <string>

class PProcInformation {
public:
    PProcInformation();
    ~PProcInformation();

    // Returns true if Intel Hybrid architecture is detected
    bool IsIntelHybridArchDetected();
    // Dumps P-core and E-core counts to console
    void DumpCoreTypes();

private:
    // Detects core types and sets member variables
    void DetectCoreTypes();
    bool intelHybridArchDetected = false;
    int pCoreCount = 0;
    int eCoreCount = 0;
};
