#pragma once

#include <cstddef>

// Kilobytes in bytes
inline unsigned long long operator ""_KB(unsigned long long kbytes) {
    return kbytes * 1024;
};

// Megabytes in bytes
inline unsigned long long operator ""_MB(unsigned long long mbytes) {
    return mbytes * 1024 * 1024;
};

// Megabytes in bytes
inline unsigned long long operator ""_GB(unsigned long long gbytes) {
    return gbytes * 1024 * 1024 * 1024;
};
