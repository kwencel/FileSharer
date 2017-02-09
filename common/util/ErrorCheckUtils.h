#ifndef FILESHARER_ERRORCHECKUTILS_H
#define FILESHARER_ERRORCHECKUTILS_H

#include <cstdio>
#include <stdexcept>

// fprintf(stderr, "Runtime error: %s returned %d at %s:%d\n", #x, retval, __FILE__, __LINE__);

#define CHK(x) do { \
    int retval = (x); \
    if (retval != 0) { \
        LOG(ERROR) << "Runtime error: " << #x << " returned " << retval << " at " << __FILE__ << ":" << __LINE__; \
    } \
} while (0)

#define CHK_POS(x) do { \
    int retval = (x); \
    if (retval < 0) { \
        LOG(ERROR) << "Runtime error: " << #x << " returned " << retval << " at " << __FILE__ << ":" << __LINE__; \
    } \
} while (0)

#define CHK_EX(x) do { \
    int retval = (x); \
    if (retval != 0) { \
        LOG(ERROR) << "Runtime error: " << #x << " returned " << retval << " at " << __FILE__ << ":" << __LINE__; \
        throw std::runtime_error(#x); \
    } \
} while (0)

#define CHK_POS_EX(x) do { \
    int retval = (x); \
    if (retval != 0) { \
        LOG(ERROR) << "Runtime error: " << #x << " returned " << retval << " at " << __FILE__ << ":" << __LINE__; \
        throw std::runtime_error(#x); \
    } \
} while (0)

#define CHK_CUSTOM(x, y) do { \
    int retval = (x); \
    if (retval != 0) { \
        LOG(ERROR) << "Runtime error: " << #x << " returned " << retval << " at " << __FILE__ << ":" << __LINE__; \
        y; \
    } \
} while (0)

#define CHK_POS_CUSTOM(x, y) do { \
    int retval = (x); \
    if (retval < 0) { \
        LOG(ERROR) << "Runtime error: " << #x << " returned " << retval << " at " << __FILE__ << ":" << __LINE__; \
        y; \
    } \
} while (0)

#define CHK_LVL(lvl, x) do { \
    int retval = (x); \
    if (retval != 0) { \
        LOG(lvl) << "Runtime error: " << #x << " returned " << retval << " at " << __FILE__ << ":" << __LINE__; \
    } \
} while (0)

#define CHK_POS_LVL(lvl, x) do { \
    int retval = (x); \
    if (retval < 0) { \
        LOG(lvl) << "Runtime error: " << #x << " returned " << retval << " at " << __FILE__ << ":" << __LINE__; \
    } \
} while (0)

#endif //FILESHARER_ERRORCHECKUTILS_H
