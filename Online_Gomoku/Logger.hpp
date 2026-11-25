#pragma once

#include <iostream>
#include <stdio.h>
#include <time.h>

#define INF 0
#define DBG 1
#define ERR 2
#define DEFAULT_LEVEL_LOG DBG

#define LOG(level, format, ...)                                                                                     \
    do                                                                                                              \
    {                                                                                                               \
        if (DEFAULT_LEVEL_LOG > level)                                                                              \
            break;                                                                                                  \
        /*获取时间戳*/                                                                                              \
        time_t t = time(NULL);                                                                                      \
        /*将时间戳转换为当地的时间*/                                                                                \
        struct tm *lt = localtime(&t);                                                                              \
        /*格式化时间*/                                                                                              \
        char buf[32];                                                                                               \
        strftime(buf, 31, "%H:%M:%S", lt);                                                                          \
        /*格式化输出到指定的文件*/                                                                                  \
        fprintf(level == ERR ? stderr : stdout, "[%s %s %d] " format "\n", buf, __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ILOG(format, ...) LOG(INF, format, ##__VA_ARGS__)
#define DLOG(format, ...) LOG(DBG, format, ##__VA_ARGS__)
#define ELOG(format, ...) LOG(ERR, "\033[31m[ERROR] " format "\033[0m", ##__VA_ARGS__)