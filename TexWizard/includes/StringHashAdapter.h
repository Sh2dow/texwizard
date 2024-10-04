#include "stdafx.h"
#include <string>
#include <vector>
#include "..\MW_Address.h"

class StringHashAdapter {
public:
    // Adapter for std::string
    static unsigned int HashString(const std::string& str) {
        // Create a local copy of the string, as we need a mutable version
        std::vector<char> mutableString(str.begin(), str.end());
        mutableString.push_back('\0');  // Ensure null termination

        // Call the original bStringHash function with the mutable string
        return bStringHash(mutableString.data());
    }

    // Adapter for const char*
    static unsigned int HashString(const char* str) {
        // Create a mutable copy of the const char* string
        std::string tempStr(str);
        return HashString(tempStr);
    }
};