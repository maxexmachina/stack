#ifndef LOG_H
#define LOG_H

#include <stdio.h>

static FILE *logFile = nullptr;
static const int LOG_OPN_FAIL = -2;
static const int LOG_WRT_FAIL = -1;

int initLog();

int writeToLog(const char *format ...);

int closeLog();

#endif
