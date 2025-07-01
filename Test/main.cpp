#include "foo.hpp"
#include "TmpFolder/bar.hpp"
#include "ThreadCheck/thread_check.hpp"
#include "logutil.hpp"

#include <thread>

int main() {
    LOG_INFO << "main: starting up";

    foo();
    bar();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    thread_check();    
    
    LOG_INFO << "main: finished";
    return 0;
}
