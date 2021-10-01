#ifndef LOG_H
#define LOG_H

#include <stdio.h>

static FILE *logFile = nullptr;

enum LogErr : int {
    LOG_OPN_FAIL = -1,
    LOG_WRT_FAIL = 0,
};

int initLog();

int writeToLog(const char *format ...);

int closeLog();

#endif
