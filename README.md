# StupidLog4C

StupidLog4C is a stupid simple, fully featured logging library for C. It
supports:

 - Standard `printf` style arguments
 - Thread safe logging without locks
 - Automatic time-based rollover (hourly, daily, monthly, or yearly)
 - Line buffered by default (log lines immediately appear in `tail -f` or similar)
 - Multiple log levels
 - Compiles with ANSI C, C99, C11, C17, etc.
 - Compile time options for:
   - Filtering by log level
   - Automatic thread-specific data teardown (requires linking with `-lpthread`)
   - Including source code file name and line number at the start of log lines

It is plain C with as simple an interface as possible, meant to be dropped in
with minimal effort. No complex class hierarchies or runtime configuration
files.

## Getting Started

Drop in `stupidlog4c.c` and `stupidlog4c.h` into your source tree, or add this
repository as a git submodule. The `Makefile` in this repository has examples
for compiling an object file, a shared object, or a static library. See the
section on compiler options for compile-time configuration.

A simple "Hello world" example with StupidLog4C:
```
#include <stdlib.h>
#include <stdio.h>

#include "stupidlog4c.h"

int main() {
	if (stupid_log_init("/tmp", "HelloWorldService.log", STUPID_LOG_HOURLY) < 0) {
		perror("stupid_log_init");
		exit(1);
	}
	atexit(stupid_log_close);

	stupid_log_info("Hello, World!");
}
```

For basic usage, make sure you call `stupid_log_init` before logging anything,
and call `stupid_log_close` before the process exits (`atexit()` is a good way to
defer this).
