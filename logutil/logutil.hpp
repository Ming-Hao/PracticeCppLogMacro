#pragma once
#include <iostream>
#include <sstream>

namespace logutil {

    enum class LogLevel { Trace, Debug, Info, Warn, Error, Unknown };
#ifdef USE_STRUCTURED_LOGGING

class logstream {
public:
    logstream(const char* file, int line, const char* func, LogLevel level);
    ~logstream();

    template <typename T>
    logstream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }

private:
    std::ostringstream oss_;
    std::ostream* stream_ = nullptr;
};

#else

class logstream {
public:
    logstream(const char*, int, const char*, LogLevel) {
        stream_ = &std::cout;
        if (level == LogLevel::Warn || level == LogLevel::Error) {
            stream_ = &std::cerr;
        }
    }
    ~logstream() {
        *oss_ << std::endl;
    }
    template <typename T>
    logstream& operator<<(const T& value) {
        *oss_ << value;
        return *this;
    }

private:
    std::ostream* stream_ = nullptr;
};

#endif // USE_STRUCTURED_LOGGING

} // namespace logutil


#ifndef FUNC_SIG
    #if defined(_MSC_VER)
        #define FUNC_SIG __FUNCSIG__
    #elif defined(__GNUC__) || defined(__clang__)
        #define FUNC_SIG __PRETTY_FUNCTION__
    #else
        #define FUNC_SIG __func__ // fallback
    #endif
#endif

// Log level macros
#define LOG_TRACE logutil::logstream(__FILE__, __LINE__, FUNC_SIG, logutil::LogLevel::Trace)
#define LOG_DEBUG logutil::logstream(__FILE__, __LINE__, FUNC_SIG, logutil::LogLevel::Debug)
#define LOG_INFO  logutil::logstream(__FILE__, __LINE__, FUNC_SIG, logutil::LogLevel::Info)
#define LOG_WARN  logutil::logstream(__FILE__, __LINE__, FUNC_SIG, logutil::LogLevel::Warn)
#define LOG_ERROR logutil::logstream(__FILE__, __LINE__, FUNC_SIG, logutil::LogLevel::Error)
