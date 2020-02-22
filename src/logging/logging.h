#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

#define ERROR   1
#define WARN    1
#define INFO    1
#define VERBOSE 1
#define DEBUG   1
#define TRACE   1

#define ERROR_TAG   "[E] "
#define WARN_TAG    "[W] "
#define INFO_TAG    "[I] "
#define VERBOSE_TAG "[V] "
#define DEBUG_TAG   "[D] "
#define TRACE_TAG   "[T] "

#define ERROR_STREAM stdout
#define WARN_STREAM stdout

#define MACRO_PICKER(_1, _2, _3, _4, _5, _6, NAME, ...)NAME

#if ERROR
#define e0(fmt) \
    fprintf(ERROR_STREAM, ERROR_TAG fmt " [%s:%d]\n", __func__, __LINE__)
#define e1(fmt, a1) \
    fprintf(ERROR_STREAM, ERROR_TAG fmt " [%s:%d]\n", a1, __func__, __LINE__)
#define e2(fmt, a1, a2) \
    fprintf(ERROR_STREAM, ERROR_TAG fmt " [%s:%d]\n", a1, a2, __func__, __LINE__)
#define e3(fmt, a1, a2, a3) \
    fprintf(ERROR_STREAM, ERROR_TAG fmt " [%s:%d]\n", a1, a2, a3, __func__, __LINE__)
#define e4(fmt, a1, a2, a3, a4) \
    fprintf(ERROR_STREAM, ERROR_TAG fmt " [%s:%d]\n", a1, a2, a3, a4, __func__, __LINE__)
#define e5(fmt, a1, a2, a3, a4, a5) \
    fprintf(ERROR_STREAM, ERROR_TAG fmt " [%s:%d]\n", a1, a2, a3, a4, a5, __func__, __LINE__)
#else
#define e0(fmt)
#define e1(fmt, a1)
#define e2(fmt, a1, a2)
#define e3(fmt, a1, a2, a3)
#define e4(fmt, a1, a2, a3, a4)
#define e5(fmt, a1, a2, a3, a4, a5)
#endif

#define e(...) MACRO_PICKER(__VA_ARGS__, e5, e4, e3, e2, e1, e0)(__VA_ARGS__)

#if WARN
#define w0(fmt) \
    fprintf(WARN_STREAM, WARN_TAG fmt " [%s:%d]\n", __func__, __LINE__)
#define w1(fmt, a1) \
    fprintf(WARN_STREAM, WARN_TAG fmt " [%s:%d]\n", a1, __func__, __LINE__)
#define w2(fmt, a1, a2) \
    fprintf(WARN_STREAM, WARN_TAG fmt " [%s:%d]\n", a1, a2, __func__, __LINE__)
#define w3(fmt, a1, a2, a3) \
    fprintf(WARN_STREAM, WARN_TAG fmt " [%s:%d]\n", a1, a2, a3, __func__, __LINE__)
#define w4(fmt, a1, a2, a3, a4) \
    fprintf(WARN_STREAM, WARN_TAG fmt " [%s:%d]\n", a1, a2, a3, a4, __func__, __LINE__)
#define w5(fmt, a1, a2, a3, a4, a5) \
    fprintf(WARN_STREAM, WARN_TAG fmt " [%s:%d]\n", a1, a2, a3, a4, a5, __func__, __LINE__)
#else
#define w0(fmt)
#define w1(fmt, a1)
#define w2(fmt, a1, a2)
#define w3(fmt, a1, a2, a3)
#define w4(fmt, a1, a2, a3, a4)
#define w5(fmt, a1, a2, a3, a4, a5)
#endif

#define w(...) MACRO_PICKER(__VA_ARGS__, w5, w4, w3, w2, w1, w0)(__VA_ARGS__)

#if INFO
#define i0(fmt) \
    printf(INFO_TAG fmt "\n")
#define i1(fmt, a1) \
    printf(INFO_TAG fmt "\n", a1)
#define i2(fmt, a1, a2) \
    printf(INFO_TAG fmt "\n", a1, a2)
#define i3(fmt, a1, a2, a3) \
    printf(INFO_TAG fmt "\n", a1, a2, a3)
#define i4(fmt, a1, a2, a3, a4) \
    printf(INFO_TAG fmt "\n", a1, a2, a3, a4)
#define i5(fmt, a1, a2, a3, a4, a5) \
    printf(INFO_TAG fmt "\n", a1, a2, a3, a4, a5)
#else
#define i0(fmt)
#define i1(fmt, a1)
#define i2(fmt, a1, a2)
#define i3(fmt, a1, a2, a3)
#define i4(fmt, a1, a2, a3, a4)
#define i5(fmt, a1, a2, a3, a4, a5)
#endif

#define i(...) MACRO_PICKER(__VA_ARGS__, i5, i4, i3, i2, i1, i0)(__VA_ARGS__)

#if DEBUG
#define d0(fmt) \
    printf(DEBUG_TAG fmt "\n")
#define d1(fmt, a1) \
    printf(DEBUG_TAG fmt "\n", a1)
#define d2(fmt, a1, a2) \
    printf(DEBUG_TAG fmt "\n", a1, a2)
#define d3(fmt, a1, a2, a3) \
    printf(DEBUG_TAG fmt "\n", a1, a2, a3)
#define d4(fmt, a1, a2, a3, a4) \
    printf(DEBUG_TAG fmt "\n", a1, a2, a3, a4)
#define d5(fmt, a1, a2, a3, a4, a5) \
    printf(DEBUG_TAG fmt "\n", a1, a2, a3, a4, a5)
#else
#define d0(fmt)
#define d1(fmt, a1)
#define d2(fmt, a1, a2)
#define d3(fmt, a1, a2, a3)
#define d4(fmt, a1, a2, a3, a4)
#define d5(fmt, a1, a2, a3, a4, a5)
#endif

#define d(...) MACRO_PICKER(__VA_ARGS__, d5, d4, d3, d2, d1, d0)(__VA_ARGS__)

#if VERBOSE
#define v0(fmt) \
    printf(VERBOSE_TAG fmt "\n")
#define v1(fmt, a1) \
    printf(VERBOSE_TAG fmt "\n", a1)
#define v2(fmt, a1, a2) \
    printf(VERBOSE_TAG fmt "\n", a1, a2)
#define v3(fmt, a1, a2, a3) \
    printf(VERBOSE_TAG fmt "\n", a1, a2, a3)
#define v4(fmt, a1, a2, a3, a4) \
    printf(VERBOSE_TAG fmt "\n", a1, a2, a3, a4)
#define v5(fmt, a1, a2, a3, a4, a5) \
    printf(VERBOSE_TAG fmt "\n", a1, a2, a3, a4, a5)
#else
#define v0(fmt)
#define v1(fmt, a1)
#define v2(fmt, a1, a2)
#define v3(fmt, a1, a2, a3)
#define v4(fmt, a1, a2, a3, a4)
#define v5(fmt, a1, a2, a3, a4, a5)
#endif

#define v(...) MACRO_PICKER(__VA_ARGS__, v5, v4, v3, v2, v1, v0)(__VA_ARGS__)

#if TRACE
#define t0(fmt) \
    printf(TRACE_TAG fmt " [%s:%d]\n", __func__, __LINE__)
#define t1(fmt, a1) \
    printf(TRACE_TAG fmt " [%s:%d]\n", a1, __func__, __LINE__)
#define t2(fmt, a1, a2) \
    printf(TRACE_TAG fmt " [%s:%d]\n", a1, a2, __func__, __LINE__)
#define t3(fmt, a1, a2, a3) \
    printf(TRACE_TAG fmt " [%s:%d]\n", a1, a2, a3, __func__, __LINE__)
#define t4(fmt, a1, a2, a3, a4) \
    printf(TRACE_TAG fmt " [%s:%d]\n", a1, a2, a3, a4__func__, __LINE__)
#define t5(fmt, a1, a2, a3, a4, a5) \
    printf(TRACE_TAG fmt " [%s:%d]\n", a1, a2, a3, a4, a5, __func__, __LINE__)
#else
#define t0(fmt)
#define t1(fmt, a1)
#define t2(fmt, a1, a2)
#define t3(fmt, a1, a2, a3)
#define t4(fmt, a1, a2, a3, a4)
#define t5(fmt, a1, a2, a3, a4, a5)
#endif

#define t(...) MACRO_PICKER(__VA_ARGS__, t5, t4, t3, t2, t1, t0)(__VA_ARGS__)

#endif // LOGGING_H
