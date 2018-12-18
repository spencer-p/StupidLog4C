/*
 * Copyright (c) 2018 Spencer Peterson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

/*
 * Change to your desired log level, or compile with
 * -DSTUPID_LOG_MIN_LEVEL=[your level here]
 */
#define STUPID_LOG_MIN_LEVEL STUPID_LOG_TRACE

/*
 * Uncomment the following define to enable file name and line number in log
 * lines.  At the compiler level, use -DSTUPID_LOG_PRINT_FILE_AND_LINE to enable
 * or -USTUPID_LOG_PRINT_FILE_AND_LINE to disable.
 *
 * #define STUPID_LOG_PRINT_FILE_AND_LINE 1
 *
 */

enum STUPID_LOG_LEVEL {
	STUPID_LOG_TRACE,
	STUPID_LOG_DEBUG,
	STUPID_LOG_INFO,
	STUPID_LOG_WARN,
	STUPID_LOG_ERROR,
	STUPID_LOG_FATAL,
};

enum STUPID_LOG_ROLLOVER {
	STUPID_LOG_YEARLY,
	STUPID_LOG_MONTHLY,
	STUPID_LOG_DAILY,
	STUPID_LOG_HOURLY,
};

int stupid_log_init(char *directory, char *prefix, enum STUPID_LOG_ROLLOVER rollover);
void stupid_log_close();
int stupid_log(const char *level, const char *format, ...) __attribute__((format (printf, 2, 3)));

#ifdef STUPID_LOG_PRINT_FILE_AND_LINE
#define stupid_log_wrap(_level, _fmt, ...) stupid_log(#_level, "%s:%d: "_fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define stupid_log_wrap(_level, _fmt, ...) stupid_log(#_level, _fmt, ##__VA_ARGS__)
#endif

#if STUPID_LOG_MIN_LEVEL <= STUPID_LOG_TRACE
#define stupid_log_trace(...) stupid_log_wrap(TRACE, __VA_ARGS__)
#endif

#if STUPID_LOG_MIN_LEVEL <= STUPID_LOG_DEBUG
#define stupid_log_debug(...) stupid_log_wrap(DEBUG, __VA_ARGS__)
#endif

#if STUPID_LOG_MIN_LEVEL <= STUPID_LOG_INFO
#define stupid_log_info(...) stupid_log_wrap(INFO, __VA_ARGS__)
#endif

#if STUPID_LOG_MIN_LEVEL <= STUPID_LOG_WARN
#define stupid_log_warn(...) stupid_log_wrap(WARN, __VA_ARGS__)
#endif

#if STUPID_LOG_MIN_LEVEL <= STUPID_LOG_ERROR
#define stupid_log_error(...) stupid_log_wrap(ERROR, __VA_ARGS__)
#endif

#if STUPID_LOG_MIN_LEVEL <= STUPID_LOG_FATAL
#define stupid_log_fatal(...) stupid_log_wrap(FATAL, __VA_ARGS__)
#endif
