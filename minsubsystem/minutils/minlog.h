#pragma once
#ifndef MINUTILS_MINLOG_H_INCLUDED
#define MINUTILS_MINLOG_H_INCLUDED

#include <cstdio>
#include <cstdarg>

#include <minutils/minhelpers.h>

#ifdef ANDROID // MinLog() on Android NDK

#include <android/log.h>

// This will push MinLog messages into LogCat.
inline void MinLog(const char *format, ...) {
  SUPPRESS_UNUSED_VARIABLE(format);
#ifdef WITH_MIN_LOG
  va_list args;
  va_start(args, format);
  __android_log_vprint(ANDROID_LOG_VERBOSE, "MinLog", format, args);
  va_end(args);
#endif // WITH_MIN_LOG
}

#else // Assume it is a generic platform with vfprintf()

inline void MinLog(const char *format, ...) {
  SUPPRESS_UNUSED_VARIABLE(format);
#ifdef WITH_MIN_LOG
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
#endif // WITH_MIN_LOG
}

#endif // Platform specific MinLog()

inline void MINLOG_ERROR(const char *message) {
  MinLog("%s:%d: Error: %s\n", __FILE__, __LINE__, message);
}

inline void MINLOG_WARNING(const char *message) {
  MinLog("%s:%d: Warning: %s\n", __FILE__, __LINE__, message);
}

#ifdef WITH_MIN_LOG
#define MIN_LOG_IF_ENABLED(call) (call);
#else // WITH_MIN_LOG
#define MIN_LOG_IF_ENABLED(call)
#endif // WITH_MIN_LOG

#endif // #ifndef MINUTILS_MINLOG_H_INCLUDED
