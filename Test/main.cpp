#include "foo.hpp"
#include "TmpFolder/bar.hpp"
#include "logutil.hpp"

int main() {
    LOG_INFO << "main: starting up";

    foo();
    bar();

    LOG_INFO << "main: finished";
    return 0;
}
