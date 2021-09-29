#include <stdio.h>

static FILE *logFile = nullptr;
static const int LOG_OPN_FAIL = -2;
static const int LOG_WRT_FAIL = -1;

int writeToLog(const char *format ...);

int closeLog();
