#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include "commons/utils/time_utils.h"

#define ERROR   1
#define WARN    1
#define INFO    1
#define VERBOSE 1
#define DEBUG   0
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
    fprintf(ERROR_STREAM, "{%lu} " ERROR_TAG fmt " [%s:%d]\n", ns(), __func__, __LINE__)
#define e1(fmt, a1) \
    fprintf(ERROR_STREAM, "{%lu} " ERROR_TAG fmt " [%s:%d]\n", ns(), a1, __func__, __LINE__)
#define e2(fmt, a1, a2) \
    fprintf(ERROR_STREAM, "{%lu} " ERROR_TAG fmt " [%s:%d]\n", ns(), a1, a2, __func__, __LINE__)
#define e3(fmt, a1, a2, a3) \
    fprintf(ERROR_STREAM, "{%lu} " ERROR_TAG fmt " [%s:%d]\n", ns(), a1, a2, a3, __func__, __LINE__)
#define e4(fmt, a1, a2, a3, a4) \
    fprintf(ERROR_STREAM, "{%lu} " ERROR_TAG fmt " [%s:%d]\n", ns(), a1, a2, a3, a4, __func__, __LINE__)
#define e5(fmt, a1, a2, a3, a4, a5) \
    fprintf(ERROR_STREAM, "{%lu} " ERROR_TAG fmt " [%s:%d]\n", ns(), a1, a2, a3, a4, a5, __func__, __LINE__)
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
    fprintf(WARN_STREAM, "{%lu} " WARN_TAG fmt " [%s:%d]\n", ns(), __func__, __LINE__)
#define w1(fmt, a1) \
    fprintf(WARN_STREAM, "{%lu} " WARN_TAG fmt " [%s:%d]\n", ns(), a1, __func__, __LINE__)
#define w2(fmt, a1, a2) \
    fprintf(WARN_STREAM, "{%lu} " WARN_TAG fmt " [%s:%d]\n", ns(), a1, a2, __func__, __LINE__)
#define w3(fmt, a1, a2, a3) \
    fprintf(WARN_STREAM, "{%lu} " WARN_TAG fmt " [%s:%d]\n", ns(), a1, a2, a3, __func__, __LINE__)
#define w4(fmt, a1, a2, a3, a4) \
    fprintf(WARN_STREAM, "{%lu} " WARN_TAG fmt " [%s:%d]\n", ns(), a1, a2, a3, a4, __func__, __LINE__)
#define w5(fmt, a1, a2, a3, a4, a5) \
    fprintf(WARN_STREAM, "{%lu} " WARN_TAG fmt " [%s:%d]\n", ns(), a1, a2, a3, a4, a5, __func__, __LINE__)
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
    printf("{%lu} " INFO_TAG fmt "\n", ns())
#define i1(fmt, a1) \
    printf("{%lu} " INFO_TAG fmt "\n", ns(), a1)
#define i2(fmt, a1, a2) \
    printf("{%lu} " INFO_TAG fmt "\n", ns(), a1, a2)
#define i3(fmt, a1, a2, a3) \
    printf("{%lu} " INFO_TAG fmt "\n", ns(), a1, a2, a3)
#define i4(fmt, a1, a2, a3, a4) \
    printf("{%lu} " INFO_TAG fmt "\n", ns(), a1, a2, a3, a4)
#define i5(fmt, a1, a2, a3, a4, a5) \
    printf("{%lu} " INFO_TAG fmt "\n", ns(), a1, a2, a3, a4, a5)
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
    printf("{%lu} " DEBUG_TAG fmt "\n", ns())
#define d1(fmt, a1) \
    printf("{%lu} " DEBUG_TAG fmt "\n", ns(), a1)
#define d2(fmt, a1, a2) \
    printf("{%lu} " DEBUG_TAG fmt "\n", ns(), a1, a2)
#define d3(fmt, a1, a2, a3) \
    printf("{%lu} " DEBUG_TAG fmt "\n", ns(), a1, a2, a3)
#define d4(fmt, a1, a2, a3, a4) \
    printf("{%lu} " DEBUG_TAG fmt "\n", ns(), a1, a2, a3, a4)
#define d5(fmt, a1, a2, a3, a4, a5) \
    printf("{%lu} " DEBUG_TAG fmt "\n", ns(), a1, a2, a3, a4, a5)
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
    printf("{%lu} " VERBOSE_TAG fmt "\n", ns())
#define v1(fmt, a1) \
    printf("{%lu} " VERBOSE_TAG fmt "\n", ns(), a1)
#define v2(fmt, a1, a2) \
    printf("{%lu} " VERBOSE_TAG fmt "\n", ns(), a1, a2)
#define v3(fmt, a1, a2, a3) \
    printf("{%lu} " VERBOSE_TAG fmt "\n", ns(), a1, a2, a3)
#define v4(fmt, a1, a2, a3, a4) \
    printf("{%lu} " VERBOSE_TAG fmt "\n", ns(), a1, a2, a3, a4)
#define v5(fmt, a1, a2, a3, a4, a5) \
    printf("{%lu} " VERBOSE_TAG fmt "\n", ns(), a1, a2, a3, a4, a5)
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
    printf("{%lu} " TRACE_TAG fmt "\n", ns())
#define t1(fmt, a1) \
    printf("{%lu} " TRACE_TAG fmt "\n", ns(), a1)
#define t2(fmt, a1, a2) \
    printf("{%lu} " TRACE_TAG fmt "\n", ns(), a1, a2)
#define t3(fmt, a1, a2, a3) \
    printf("{%lu} " TRACE_TAG fmt "\n", ns(), a1, a2, a3)
#define t4(fmt, a1, a2, a3, a4) \
    printf("{%lu} " TRACE_TAG fmt "\n", ns(), a1, a2, a3, a4)
#define t5(fmt, a1, a2, a3, a4, a5) \
    printf("{%lu} " TRACE_TAG fmt "\n", ns(), a1, a2, a3, a4, a5)
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
