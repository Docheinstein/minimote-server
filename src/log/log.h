#ifndef LOG_H
#define LOG_H

#include <stdbool.h>
#include <stdio.h>
#include "commons/utils/time_utils.h"

#ifndef LOG_ERROR
#define LOG_ERROR 1
#endif // LOG_ERROR

#ifndef LOG_WARN
#define LOG_WARN 1
#endif // LOG_WARN

#ifndef LOG_INFO
#define LOG_INFO 1
#endif // LOG_INFO

#ifndef LOG_VERBOSE
#define LOG_VERBOSE 1
#endif // LOG_VERBOSE

#ifndef LOG_DEBUG
#define LOG_DEBUG 0
#endif // LOG_DEBUG

#define ERROR_STREAM stderr
#define WARN_STREAM stderr

#define ERROR_PREFIX "ERROR: "
#define WARN_PREFIX "WARN: "

#define LOG_MACRO_PICKER(_1, _2, _3, _4, _5, _6, NAME, ...) NAME

extern bool verbose;

void log_verbose_enable(bool enabled);
bool log_verbose_is_enabled();

#if LOG_ERROR
#define e0(fmt) \
    fprintf(ERROR_STREAM, ERROR_PREFIX fmt " [%s:%d]\n", __func__, __LINE__)
#define e1(fmt, a1) \
    fprintf(ERROR_STREAM, ERROR_PREFIX fmt " [%s:%d]\n", a1, __func__, __LINE__)
#define e2(fmt, a1, a2) \
    fprintf(ERROR_STREAM, ERROR_PREFIX fmt " [%s:%d]\n", a1, a2, __func__, __LINE__)
#define e3(fmt, a1, a2, a3) \
    fprintf(ERROR_STREAM, ERROR_PREFIX fmt " [%s:%d]\n", a1, a2, a3, __func__, __LINE__)
#define e4(fmt, a1, a2, a3, a4) \
    fprintf(ERROR_STREAM, ERROR_PREFIX fmt " [%s:%d]\n", a1, a2, a3, a4, __func__, __LINE__)
#define e5(fmt, a1, a2, a3, a4, a5) \
    fprintf(ERROR_STREAM, ERROR_PREFIX fmt " [%s:%d]\n", a1, a2, a3, a4, a5, __func__, __LINE__)
#else
#define e0(fmt)
#define e1(fmt, a1)
#define e2(fmt, a1, a2)
#define e3(fmt, a1, a2, a3)
#define e4(fmt, a1, a2, a3, a4)
#define e5(fmt, a1, a2, a3, a4, a5)
#endif

#define e(...) LOG_MACRO_PICKER(__VA_ARGS__, e5, e4, e3, e2, e1, e0)(__VA_ARGS__)

#if LOG_WARN
#define w0(fmt) \
    fprintf(WARN_STREAM, WARN_PREFIX fmt " [%s:%d]\n", __func__, __LINE__)
#define w1(fmt, a1) \
    fprintf(WARN_STREAM, WARN_PREFIX fmt " [%s:%d]\n", a1, __func__, __LINE__)
#define w2(fmt, a1, a2) \
    fprintf(WARN_STREAM, WARN_PREFIX fmt " [%s:%d]\n", a1, a2, __func__, __LINE__)
#define w3(fmt, a1, a2, a3) \
    fprintf(WARN_STREAM, WARN_PREFIX fmt " [%s:%d]\n", a1, a2, a3, __func__, __LINE__)
#define w4(fmt, a1, a2, a3, a4) \
    fprintf(WARN_STREAM, WARN_PREFIX fmt " [%s:%d]\n", a1, a2, a3, a4, __func__, __LINE__)
#define w5(fmt, a1, a2, a3, a4, a5) \
    fprintf(WARN_STREAM, WARN_PREFIX fmt " [%s:%d]\n", a1, a2, a3, a4, a5, __func__, __LINE__)
#else
#define w0(fmt)
#define w1(fmt, a1)
#define w2(fmt, a1, a2)
#define w3(fmt, a1, a2, a3)
#define w4(fmt, a1, a2, a3, a4)
#define w5(fmt, a1, a2, a3, a4, a5)
#endif

#define w(...) LOG_MACRO_PICKER(__VA_ARGS__, w5, w4, w3, w2, w1, w0)(__VA_ARGS__)

#if LOG_INFO
#define i0(fmt) \
    printf(fmt "\n")
#define i1(fmt, a1) \
    printf(fmt "\n", a1)
#define i2(fmt, a1, a2) \
    printf(fmt "\n", a1, a2)
#define i3(fmt, a1, a2, a3) \
    printf(fmt "\n", a1, a2, a3)
#define i4(fmt, a1, a2, a3, a4) \
    printf(fmt "\n", a1, a2, a3, a4)
#define i5(fmt, a1, a2, a3, a4, a5) \
    printf(fmt "\n", a1, a2, a3, a4, a5)
#else
#define i0(fmt)
#define i1(fmt, a1)
#define i2(fmt, a1, a2)
#define i3(fmt, a1, a2, a3)
#define i4(fmt, a1, a2, a3, a4)
#define i5(fmt, a1, a2, a3, a4, a5)
#endif

#define i(...) LOG_MACRO_PICKER(__VA_ARGS__, i5, i4, i3, i2, i1, i0)(__VA_ARGS__)


#if LOG_VERBOSE
#define v0(fmt) \
    if (verbose) printf(fmt "\n")
#define v1(fmt, a1) \
    if (verbose) printf(fmt "\n", a1)
#define v2(fmt, a1, a2) \
    if (verbose) printf(fmt "\n", a1, a2)
#define v3(fmt, a1, a2, a3) \
    if (verbose) printf(fmt "\n", a1, a2, a3)
#define v4(fmt, a1, a2, a3, a4) \
    if (verbose) printf(fmt "\n", a1, a2, a3, a4)
#define v5(fmt, a1, a2, a3, a4, a5) \
    if (verbose) printf(fmt "\n", a1, a2, a3, a4, a5)
#else
#define v0(fmt)
#define v1(fmt, a1)
#define v2(fmt, a1, a2)
#define v3(fmt, a1, a2, a3)
#define v4(fmt, a1, a2, a3, a4)
#define v5(fmt, a1, a2, a3, a4, a5)
#endif

#define v(...) LOG_MACRO_PICKER(__VA_ARGS__, v5, v4, v3, v2, v1, v0)(__VA_ARGS__)


#if LOG_DEBUG
#define d0(fmt) \
    printf(fmt "\n")
#define d1(fmt, a1) \
    printf(fmt "\n", a1)
#define d2(fmt, a1, a2) \
    printf(fmt "\n", a1, a2)
#define d3(fmt, a1, a2, a3) \
    printf(fmt "\n", a1, a2, a3)
#define d4(fmt, a1, a2, a3, a4) \
    printf(fmt "\n", a1, a2, a3, a4)
#define d5(fmt, a1, a2, a3, a4, a5) \
    printf(fmt "\n", a1, a2, a3, a4, a5)
#else
#define d0(fmt)
#define d1(fmt, a1)
#define d2(fmt, a1, a2)
#define d3(fmt, a1, a2, a3)
#define d4(fmt, a1, a2, a3, a4)
#define d5(fmt, a1, a2, a3, a4, a5)
#endif

#define d(...) LOG_MACRO_PICKER(__VA_ARGS__, d5, d4, d3, d2, d1, d0)(__VA_ARGS__)

#endif // LOG_H
