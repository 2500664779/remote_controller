#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <string.h>
#include <errno.h>

//反斜杠表示,这一行和下一行是相连的.
#define PRINT(title, file, func, line, tick, fmt, ...) \
	printf("[%s] - %s:%s:%d %s, " fmt, title, file, func, line, tick, ##__VA_ARGS__)
#define PRINT_INFO(title, tick, fmt, ...) \
	PRINT(title, __FILE__, __FUNCTION__, __LINE__, tick, fmt, ##__VA_ARGS__)

#define DEBUG
#ifdef DEBUG
#define trace(fmt, ...) PRINT_INFO("trace", "", fmt, ##__VA_ARGS__)
#define error(fmt, ...) PRINT_INFO("error", "", fmt, ##__VA_ARGS__)
#define errsys(fmt, ...) PRINT_INFO("error", strerror(errno), fmt, ##__VA_ARGS__)
#else
#define trace(fmt, ...)
#define error(fmt, ...)
#define errsys(fmt, ...)
#endif
#endif