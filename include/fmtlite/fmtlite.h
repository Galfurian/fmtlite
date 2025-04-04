/// @file fmtlite.h
/// @brief Minimal freestanding implementation of `vsnprintf`, `snprintf`, and `sprintf`.
/// @author
/// Enrico Fraccaroli (enrico.fraccaroli@univr.it)
/// @copyright
/// MIT License - see LICENSE file for details.
/// @version 1.0

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stddef.h>

/// @brief Writes formatted output to a string (size-limited, safe).
/// @param str Destination buffer.
/// @param size Maximum number of bytes (including null terminator).
/// @param format Format string (like printf).
/// @param args Argument list.
/// @return Number of characters that *would* have been written, excluding null byte.
int fmtlite_vsnprintf(char *str, size_t size, const char *format, va_list args);

/// @brief Writes formatted output to a string (size-limited, safe).
/// @param str Destination buffer.
/// @param size Maximum number of bytes (including null terminator).
/// @param format Format string (like printf).
/// @return Number of characters that *would* have been written, excluding null byte.
int fmtlite_snprintf(char *str, size_t size, const char *format, ...);

/// @brief Writes formatted output to a string (unbounded, unsafe).
/// @param str Destination buffer.
/// @param format Format string (like printf).
/// @return Number of characters written (excluding null byte).
int fmtlite_sprintf(char *str, const char *format, ...);

#ifdef __cplusplus
}
#endif
