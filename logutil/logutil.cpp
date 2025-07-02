#include "logutil.hpp"
#include <iomanip>
#include <vector>
#include <algorithm>
#include <sstream>
#include <thread>
#include <chrono>

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
static std::mutex cerr_mutex;

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

inline bool isLogLevelDisabled(LogLevel level) {
#ifndef LOGUTIL_LOG_LEVELS_TRACE
    if (level == LogLevel::Trace) {
        return true;
    }
#endif

#ifndef LOGUTIL_LOG_LEVELS_DEBUG
    if (level == LogLevel::Debug) {
        return true;
    }
#endif

#ifndef LOGUTIL_LOG_LEVELS_INFO
    if (level == LogLevel::Info) {
        return true;
    }
#endif

#ifndef LOGUTIL_LOG_LEVELS_WARN
    if (level == LogLevel::Warn) {
        return true;
    }
#endif

#ifndef LOGUTIL_LOG_LEVELS_ERROR
    if (level == LogLevel::Error) {
        return true;
    }
#endif
    return false;
}

#ifdef LOGUTIL_INCLUDE_TIMESTAMP
static void getTimestamp(std::ostream& os) {
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    auto now_tm = *std::localtime(&now_time);
    char timestamp[20];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &now_tm);

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    os << "[" << timestamp << "." << std::setw(3) << std::setfill('0') << ms.count() << "]";
}
#endif

inline static void appendTimestampIfEnabled(std::ostream& os) {
#ifdef LOGUTIL_INCLUDE_TIMESTAMP
    getTimestamp(os);
    os << " ";
#endif
}

inline static void appendThreadIdIfEnabled(std::ostream& os) {
#ifdef LOGUTIL_INCLUDE_THREAD_ID
    os << "[ThreadId : " << std::this_thread::get_id() << "] ";
#endif
}

inline static void appendFileLineIfEnabled(std::ostream& os, const char* file, int line) {
#ifdef LOGUTIL_INCLUDE_FILE
    #ifdef LOGUTIL_SHORTEN_PATH
        os << shorten_file_path_with_ellipsis(file, LOGUTIL_PATH_DEPTH) << " : ";
    #else
        os << file << " : ";
    #endif
    #ifdef LOGUTIL_INCLUDE_LINE
        os << line << " ";
    #endif
#endif
}

inline static void appendFunctionIfEnabled(std::ostream& os, const char* func) {
#ifdef LOGUTIL_INCLUDE_FUNC
    os << "in " << func;
#endif
}


inline static void setupLogLabel(std::ostream& os, LogLevel level) {
    const LogLevelInfo* info = findLogLevelInfo(level);
    int maxLabelWidth = getMaxLabelWidth();

    const char* label = "?";
    size_t labelLen = 1;

    if (info != nullptr) {
        label = info->label;
        labelLen = info->labelLen;
    }

    os << "[" << label << "]";
    for (int i = 0; i < maxLabelWidth - labelLen; ++i) {
        os << ' ';
    }
}

logstream::logstream(const char* file, int line, const char* func, LogLevel level)
{
    if (isLogLevelDisabled(level)) {
        stream_ = nullptr;
        return;
    }

    stream_ = &std::cout;
    if (level == LogLevel::Warn || level == LogLevel::Error) {
        stream_ = &std::cerr;
    }

    setupLogLabel(oss_, level);
    appendTimestampIfEnabled(oss_);
    appendThreadIdIfEnabled(oss_);
    appendFileLineIfEnabled(oss_, file, line);
    appendFunctionIfEnabled(oss_, func);
    
    oss_ << " - ";
}

logstream::~logstream() {
    if (stream_ == nullptr) {
        return;
    }
    if (stream_ == &std::cerr) {
        std::lock_guard<std::mutex> lock(cerr_mutex);
        *stream_ << oss_.str() << std::endl;
    }
    else if (stream_ == &std::cout) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        *stream_ << oss_.str() << std::endl;
    }
}

} // namespace logutil

#endif // USE_STRUCTURED_LOGGING