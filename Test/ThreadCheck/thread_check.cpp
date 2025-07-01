#include "thread_check.hpp"

#include <thread>
#include "logutil.hpp"
#include "../foo.hpp"

void thread_check() {
    std::thread new_thread([] {
        for (int i = 0; i < 5; i++) {
            LOG_INFO << "thread_check: " << i;
        }
        // sleep for 1 millisecond
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        foo();
    });

    new_thread.join();
}