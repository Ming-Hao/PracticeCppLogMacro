#include "logutil.hpp"
#include <iomanip>
#include <vector>
#include <algorithm>
#include <sstream>

#ifndef LOG_PREFIX_INFO
#define LOG_PREFIX_INFO INFO
#endif

#ifndef LOG_PREFIX_WARN
#define LOG_PREFIX_WARN WARN
#endif

#ifndef LOG_PREFIX_ERROR
#define LOG_PREFIX_ERROR ERROR
#endif

#ifndef LOGUTIL_PATH_DEPTH
#define LOGUTIL_PATH_DEPTH 5
#endif

#define STRINGIFY(x) #x
#define EXPAND_AND_STRINGIFY(x) STRINGIFY(x)

#ifdef USE_STRUCTURED_LOGGING

#include <mutex>
static std::mutex cout_mutex;

inline static std::string shorten_file_path_with_ellipsis(const char* fullPath, int depth = 2) {
    std::string path(fullPath);
    std::replace(path.begin(), path.end(), '\\', '/'); // Normalize separators for cross-platform support

    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string segment;

    // Split path by '/'
    while (std::getline(ss, segment, '/')) {
        if (!segment.empty()) parts.push_back(segment);
    }

    // If path depth is small or within limit, return as-is
    if (parts.size() <= depth) {
        return path;
    }

    // Compose abbreviated path: <depth-1 parts>/.../<last>
    std::string result;
    for (int i = 0; i < depth - 1 && i < static_cast<int>(parts.size()); ++i) {
        result += parts[i] + "/";
    }
    result += ".../" + parts.back();
    return result;
}

inline static int getMaxLabelWidth() {
    static const int width = [] {
        const char* labels[] = { EXPAND_AND_STRINGIFY(LOG_PREFIX_INFO),
                                 EXPAND_AND_STRINGIFY(LOG_PREFIX_WARN),
                                 EXPAND_AND_STRINGIFY(LOG_PREFIX_ERROR) };
        int maxLen = 0;
        for (auto label : labels)
            maxLen = std::max<int>(maxLen, static_cast<int>(std::strlen(label)));
        return maxLen;
    }();
    return width;
}

namespace logutil {

logstream::logstream(const char* file, int line, const char* func, LogLevel level)
{
    static const char* InfoPrefixCache  = EXPAND_AND_STRINGIFY(LOG_PREFIX_INFO);
    static const char* WarnPrefixCache  = EXPAND_AND_STRINGIFY(LOG_PREFIX_WARN);
    static const char* ErrorPrefixCache = EXPAND_AND_STRINGIFY(LOG_PREFIX_ERROR);
    
    static const int InfoLen  = static_cast<int>(std::strlen(InfoPrefixCache));
    static const int WarnLen  = static_cast<int>(std::strlen(WarnPrefixCache));
    static const int ErrorLen = static_cast<int>(std::strlen(ErrorPrefixCache));
    
    int maxLabelWidth = getMaxLabelWidth();

    const char* levelConstChar = "?";
    int labelLen = 0;
    switch (level) {
        case LogLevel::Info:
            levelConstChar = InfoPrefixCache;
            labelLen = InfoLen;
            break;
        case LogLevel::Warn:
            levelConstChar = WarnPrefixCache;
            labelLen = WarnLen;
            break;
        case LogLevel::Error:
            levelConstChar = ErrorPrefixCache;
            labelLen = ErrorLen;
            break;
        case LogLevel::Unknown:
        default:
            break;
    }
    oss_ << "[" << levelConstChar << "]";
    for (int i = 0; i < maxLabelWidth - labelLen; ++i) {
        oss_ << ' ';
    }
#ifdef LOGUTIL_INCLUDE_FILE
    #ifdef LOGUTIL_SHORTEN_PATH
        oss_ << " " << shorten_file_path_with_ellipsis(file, LOGUTIL_PATH_DEPTH) << " : ";
    #else
        oss_ << " " << file << " : ";
    #endif
    #ifdef LOGUTIL_INCLUDE_LINE
        oss_ << line;
    #endif
#endif
#ifdef LOGUTIL_INCLUDE_FUNC
    oss_ << " in " << func;
#endif
    oss_ << " - ";
}

logstream::~logstream() {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << oss_.str() << std::endl;
}

} // namespace logutil

#endif // USE_STRUCTURED_LOGGING