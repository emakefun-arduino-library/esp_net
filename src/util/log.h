#pragma once

#ifndef LOG_ENABLE
#define LOG_ENABLE (0)
#endif

#if LOG_ENABLE
#include "clog.h"
#else
#define CLOG(x, ...) (void(nullptr))
#endif