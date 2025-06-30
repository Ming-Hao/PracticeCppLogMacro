#include "bar.hpp"
#include "logutil.hpp"

void helper() {
    LOG_TRACE << "helper++";
    LOG_ERROR << "util: inside helper()";
    LOG_TRACE << "helper--";
}

void bar() {
    LOG_WARN << "bar: starting task";
    helper();
    []() {
        LOG_TRACE << "lambda++";
        LOG_INFO << "bar: inside lambda";
        LOG_TRACE << "lambda--";
    }();
    LOG_INFO << "bar: task done";
}
