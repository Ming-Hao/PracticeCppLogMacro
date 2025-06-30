# logutil

This is just a small personal practice project.

The goal is to create a simple logging utility that can be easily added to any CMake-based C++ project.  
It's meant to replace the usual `std::cout << ...` calls with something like:

```cpp
LOG_INFO  << "some message";
LOG_WARN  << "warn message";
LOG_ERROR << "error message";
LOG_DEBUG << "debug details";
LOG_TRACE << "step-by-step trace";
```

And the output will automatically include useful context like `__FILE__`, `__LINE__`, and the function signature (depending on compiler, e.g. `__PRETTY_FUNCTION__` or `__FUNCSIG__`).

---

## How to Use

1. Drop this folder into your project, e.g., `external/logutil/`
2. In your `CMakeLists.txt`, add:

```cmake
add_subdirectory(external/logutil)
target_link_libraries(your_target PRIVATE logutil)
```

3. Then use it in your code:

```cpp
#include "logutil/logstream.hpp"

void foo() {
    LOG_INFO << "Hello";
}
```

---

## CMake Options

This utility supports a few options that can be configured via `-D` flags when running `cmake`:

| Option                    | Default | Description                                |
|--------------------------|---------|--------------------------------------------|
| `LOGUTIL_USE_STRUCTURED_LOGGING` | `ON`    | Enables structured log output              |
| `LOGUTIL_INCLUDE_FILE`   | `ON`    | Includes file path                         |
| `LOGUTIL_INCLUDE_LINE`   | `ON`    | Includes line number                       |
| `LOGUTIL_INCLUDE_FUNC`   | `ON`    | Includes function signature                |
| `LOGUTIL_SHORTEN_PATH`   | `ON`    | Shortens file path with `...`              |
| `LOGUTIL_PATH_DEPTH`     | `5`     | Number of path segments to keep when shortening |

### Log Filtering

| Option                    | Default | Controls log visibility for...             |
|--------------------------|---------|--------------------------------------------|
| `LOGUTIL_VERBOSE_LEVEL`  | `2`     | Verbosity level: `0=trace`, `1=debug`, `2=info` |
| `LOGUTIL_ENABLE_WARN`    | `ON`    | Enables output for `LOG_WARN`              |
| `LOGUTIL_ENABLE_ERROR`   | `ON`    | Enables output for `LOG_ERROR`             |

Levels outside the allowed range are completely excluded at compile-time.

### Example:

```bash
cmake -S . -B build \
  -DLOGUTIL_VERBOSE_LEVEL=1 \
  -DLOGUTIL_ENABLE_WARN=ON \
  -DLOGUTIL_ENABLE_ERROR=ON
```

This enables `DEBUG`, `INFO`, `WARN`, and `ERROR`, but excludes `TRACE`.

---

## Customizing Log Level Prefixes

By default, log levels are shown as `[TRACE]`, `[DEBUG]`, `[INFO]`, `[WARN]`, and `[ERROR]`.  
You can override the labels by defining the following macros when linking `logutil`:

```cmake
target_compile_definitions(myapp PRIVATE
    LOG_PREFIX_TRACE=T
    LOG_PREFIX_DEBUG=D
    LOG_PREFIX_INFO=I
    LOG_PREFIX_WARN=W
    LOG_PREFIX_ERROR=E
)
```

This will change the output to look like:

```
[I]   src/.../main.cpp : 10 in foo - message
[W]   src/.../main.cpp : 11 in foo - warning
[E]   src/.../main.cpp : 12 in foo - error
```

You can use any string or symbol as the prefix.  
This is useful for customizing the log format or aligning with existing logging systems.

---

## Notes

- This is a small experiment and not meant for production use.
- Mainly written for learning purposes — feel free to modify as needed.

## Planned Features / TODO

- [ ] Include timestamp in each log line
- [ ] Show current thread ID in structured logs