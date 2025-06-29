#include "bar.hpp"
#include "logutil.hpp"

void helper() {
    LOG_ERROR << "util: inside helper()";
}

void bar() {
    LOG_WARN << "bar: starting task";
    helper();
    []() {
        LOG_INFO << "bar: inside lambda";
    }();
    LOG_INFO << "bar: task done";
}
