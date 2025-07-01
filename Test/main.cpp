#include "foo.hpp"
#include "TmpFolder/bar.hpp"
#include "ThreadCheck/thread_check.hpp"
#include "logutil.hpp"

int main() {
    LOG_INFO << "main: starting up";

    foo();
    bar();
    thread_check();
    
    LOG_INFO << "main: finished";
    return 0;
}
