#include "logutil.hpp"
#include <iomanip>
#include <vector>
#include <algorithm>
#include <sstream>

#ifndef LOG_PREFIX_TRACE
#define LOG_PREFIX_TRACE TRACE
#endif

#ifndef LOG_PREFIX_DEBUG
#define LOG_PREFIX_DEBUG DEBUG
#endif

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
        const char* labels[] = { EXPAND_AND_STRINGIFY(LOG_PREFIX_TRACE),
                                 EXPAND_AND_STRINGIFY(LOG_PREFIX_DEBUG),
                                 EXPAND_AND_STRINGIFY(LOG_PREFIX_INFO),
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

struct LogLevelInfo {
    const char* label;
    size_t      labelLen;
    LogLevel    level;
};

static const LogLevelInfo kLogLevelTable[] = {
    {EXPAND_AND_STRINGIFY(LOG_PREFIX_TRACE), std::strlen(EXPAND_AND_STRINGIFY(LOG_PREFIX_TRACE)), LogLevel::Trace},
    {EXPAND_AND_STRINGIFY(LOG_PREFIX_DEBUG), std::strlen(EXPAND_AND_STRINGIFY(LOG_PREFIX_DEBUG)), LogLevel::Debug},
    {EXPAND_AND_STRINGIFY(LOG_PREFIX_INFO),  std::strlen(EXPAND_AND_STRINGIFY(LOG_PREFIX_INFO)),  LogLevel::Info},
    {EXPAND_AND_STRINGIFY(LOG_PREFIX_WARN),  std::strlen(EXPAND_AND_STRINGIFY(LOG_PREFIX_WARN)),  LogLevel::Warn},
    {EXPAND_AND_STRINGIFY(LOG_PREFIX_ERROR), std::strlen(EXPAND_AND_STRINGIFY(LOG_PREFIX_ERROR)), LogLevel::Error},
    // extend this table as needed
};

static const LogLevelInfo* findLogLevelInfo(LogLevel level) {
    for (const auto& entry : kLogLevelTable) {
        if (entry.level == level) return &entry;
    }
    return nullptr;
}

logstream::lgstream(const char* file, int line, const char* func, LogLevel level)
{
#ifndef LOGUTIL_LOG_LEVELS_TRACE
    if (level == LogLevel::Trace) {
        stream_ = nullptr;
        return;
    }
#endif

#ifndef LOGUTIL_LOG_LEVELS_DEBUG
    if (level == LogLevel::Debug) {
        stream_ = nullptr;
        return;
    }
#endif

#ifndef LOGUTIL_LOG_LEVELS_INFO
    if (level == LogLevel::Info) {
        stream_ = nullptr;
        return;
    }
#endif

#ifndef LOGUTIL_LOG_LEVELS_WARN
    if (level == LogLevel::Warn) {
        stream_ = nullptr;
        return;
    }
#endif

#ifndef LOGUTIL_LOG_LEVELS_ERROR
    if (level == LogLevel::Error) {
        stream_ = nullptr;
        return;
    }
#endif

    stream_ = &std::cout;
    if (level == LogLevel::Warn || level == LogLevel::Error) {
        stream_ = &std::cerr;
    }
    const LogLevelInfo* info = findLogLevelInfo(level);
    int maxLabelWidth = getMaxLabelWidth();

    const char* label = "?";
    size_t labelLen = 1;

    if (info != nullptr) {
        label = info->label;
        labelLen = info->labelLen;
    }

    oss_ << "[" << label << "]";
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
    if (stream_ == nullptr) {
        return;
    }
    std::lock_guard<std::mutex> lock(cout_mutex);
    *stream_ << oss_.str() << std::endl;
}

} // namespace logutil

#endif // USE_STRUCTURED_LOGGING