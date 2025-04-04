/// @file fmtlite.h
/// @brief Minimal freestanding implementation of `vsnprintf`, `snprintf`, and `sprintf`.
/// @author
/// Enrico Fraccaroli (enrico.fraccaroli@univr.it)
/// @copyright
/// MIT License - see LICENSE file for details.

#include <fmtlite/fmtlite.h>

#define FLAGS_ZEROPAD   (1U << 0U) ///< Fill zeros before the number.
#define FLAGS_LEFT      (1U << 1U) ///< Left align the value.
#define FLAGS_PLUS      (1U << 2U) ///< Print the plus sign.
#define FLAGS_SPACE     (1U << 3U) ///< If positive add a space instead of the plus sign.
#define FLAGS_UPPERCASE (1U << 4U) ///< Print uppercase.
#define FLAGS_SIGN      (1U << 5U) ///< Print the sign.
#define FLAGS_NEGATIVE  (1U << 6U) ///< Negative number flag.

double __round(double x) { return (x >= 0.0) ? (double)((int)(x + 0.5)) : (double)((int)(x - 0.5)); }

double __pow(double base, int exp)
{
    double result = 1.0;
    int negative  = 0;

    if (exp < 0) {
        negative = 1;
        exp      = -exp;
    }

    while (exp) {
        if (exp & 1)
            result *= base;
        base *= base;
        exp >>= 1;
    }

    return negative ? 1.0 / result : result;
}

/// @brief Internal function to emit a character.
/// @param buf Current pointer in the buffer.
/// @param end Pointer to the end of the buffer.
/// @param c The character to emit.
static inline void __emit_char(char **buf, char *end, char c)
{
    if (*buf && *buf < end) {
        *(*buf)++ = c;
    } else if (*buf) {
        (*buf)++;
    }
}

/// @brief Internal function to emit padding characters.
/// @param buf Current pointer in the buffer.
/// @param end Pointer to the end of the buffer.
/// @param padding The number of padding characters to emit.
/// @param padchar The character to use for padding.
static void __emit_padding(char **buf, char *end, int padding, char padchar)
{
    while (padding-- > 0) {
        __emit_char(buf, end, padchar);
    }
}

/// @brief Internal function to emit a number in a specific base.
/// @param buffer Buffer to store the number.
/// @param buflen Length of the buffer.
/// @param num The number to format.
/// @param base The base to use for formatting (e.g., 10 for decimal, 16 for hexadecimal).
/// @param precision The minimum number of digits to print.
/// @param flags Formatting flags.
/// @return The length of the formatted number.
static int __emit_number(char *buffer, size_t buflen, unsigned long num, int base, int precision, int flags)
{
    size_t len = 0;
    // Reserve space for prefix.
    buflen -= (flags & (FLAGS_NEGATIVE | FLAGS_PLUS | FLAGS_SPACE));
    // Convert number to string (in reverse).
    do {
        buffer[len++] = ((flags & FLAGS_UPPERCASE) ? "0123456789ABCDEF" : "0123456789abcdef")[num % base];
        num /= base;
    } while ((num > 0) && (len < buflen));
    // Apply precision (zero padding).
    if (precision > 0) {
        while ((len < precision) && (len < buflen)) {
            buffer[len++] = '0';
        }
    }
    // Handle sign/prefix.
    if (flags & FLAGS_NEGATIVE) {
        buffer[len++] = '-';
    } else if (flags & FLAGS_PLUS) {
        buffer[len++] = '+';
    } else if (flags & FLAGS_SPACE) {
        buffer[len++] = ' ';
    }
    return len;
}

/// @brief Internal function to handle string formatting.
/// @param buf Current pointer in the buffer.
/// @param end Pointer to the end of the buffer.
/// @param str The string to format.
/// @param width The minimum width of the output.
/// @param precision The maximum number of characters to print.
/// @param flags Formatting flags.
static void __format_string(char **buf, char *end, const char *str, int width, int precision, int flags)
{
    // Handle null string case.
    if (str == NULL) {
        str = "(null)";
    }
    int len       = 0;
    const char *s = str;
    // Directly calculate the length of the string.
    while (*s) {
        len++;
        s++;
    }
    // If precision is set, limit the length to precision.
    if (precision >= 0 && precision < len) {
        len = precision;
    }
    // Calculate remaining width for padding.
    int padding = width - len;
    // Apply right padding if not left aligned.
    if (!(flags & FLAGS_LEFT)) {
        __emit_padding(buf, end, padding, ' ');
    }
    // Copy the string into the buffer.
    while (*str && (len-- > 0)) {
        __emit_char(buf, end, *str++);
    }
    // Apply left padding if left aligned.
    if (flags & FLAGS_LEFT) {
        __emit_padding(buf, end, padding, ' ');
    }
}

/// @brief Internal function to handle character formatting.
/// @param buf Current pointer in the buffer.
/// @param end Pointer to the end of the buffer.
/// @param c The character to format.
/// @param width The minimum width of the output.
/// @param flags Formatting flags.
static void __format_char(char **buf, char *end, char c, int width, int flags)
{
    // Calculate remaining width.
    int padding = width - 1;
    // Apply right padding before the character if right-aligned.
    if (!(flags & FLAGS_LEFT)) {
        __emit_padding(buf, end, padding, ' ');
    }
    // Insert character.
    __emit_char(buf, end, c);
    // Apply left padding only if FLAGS_LEFT is set.
    if (flags & FLAGS_LEFT) {
        __emit_padding(buf, end, padding, ' ');
    }
}

/// @brief Internal function to handle integer formatting.
/// @param buf Current pointer in the buffer.
/// @param end Pointer to the end of the buffer.
/// @param num The number to format.
/// @param base The base to use for formatting (e.g., 10 for decimal, 16 for hexadecimal).
/// @param width The minimum width of the output.
/// @param precision The minimum number of digits to print.
/// @param flags Formatting flags.
static void __format_integer(char **buf, char *end, long num, int base, int width, int precision, int flags)
{
    char tmp[32] = {0};
    unsigned long unum;
    if (num < 0) {
        unum = -num;
        flags |= FLAGS_NEGATIVE;
    } else {
        unum = num;
    }
    // Convert number to string in reverse.
    int len     = __emit_number(tmp, sizeof(tmp) - 1, unum, 10, precision, flags);
    // Calculate remaining width.
    int padding = width - len;
    // Apply right padding before the character if right-aligned.
    if (!(flags & FLAGS_LEFT)) {
        __emit_padding(buf, end, padding, (flags & FLAGS_ZEROPAD) ? '0' : ' ');
    }
    // Copy the converted number in correct order.
    while (len-- > 0) {
        __emit_char(buf, end, tmp[len]);
    }
    // Apply left padding only if FLAGS_LEFT is set.
    if (flags & FLAGS_LEFT) {
        __emit_padding(buf, end, padding, ' ');
    }
}

/// @brief Internal function to handle unsigned integer formatting.
/// @param buf Current pointer in the buffer.
/// @param end Pointer to the end of the buffer.
/// @param num The number to format.
/// @param base The base to use for formatting (e.g., 10 for decimal, 16 for hexadecimal).
/// @param width The minimum width of the output.
/// @param precision The minimum number of digits to print.
/// @param flags Formatting flags.
static void __format_unsigned(char **buf, char *end, unsigned long num, int base, int width, int precision, int flags)
{
    char tmp[32] = {0};
    // Convert number to string (reverse order).
    int len      = __emit_number(tmp, sizeof(tmp) - 1, num, base, precision, flags);
    // Calculate remaining width.
    int padding  = width - len;
    // Apply right padding before the character if right-aligned.
    if (!(flags & FLAGS_LEFT)) {
        __emit_padding(buf, end, padding, (flags & FLAGS_ZEROPAD) ? '0' : ' ');
    }
    // Copy number to buffer in correct order.
    while (len-- > 0) {
        __emit_char(buf, end, tmp[len]);
    }
    // Apply left padding only if FLAGS_LEFT is set.
    if (flags & FLAGS_LEFT) {
        __emit_padding(buf, end, padding, ' ');
    }
}

/// @brief Internal function to handle floating-point formatting.
/// @param buf Current pointer in the buffer.
/// @param end Pointer to the end of the buffer.
/// @param num The floating-point number to format.
/// @param width The minimum width of the output.
/// @param precision The number of digits after the decimal point.
/// @param flags Formatting flags.
static void __format_float(char **buf, char *end, double num, int width, int precision, int flags)
{
    if (precision < 0) {
        precision = 6;
    }
    // Prepare temporary buffer
    char float_buf[64] = {0};
    char *tmp          = float_buf;
    char *tmp_end      = float_buf + sizeof(float_buf) - 1;
    // Format sign (we handle it ourselves instead of passing via flags)
    if (num < 0.0) {
        *tmp++ = '-';
        num    = -num;
    } else if (flags & FLAGS_PLUS) {
        *tmp++ = '+';
    } else if (flags & FLAGS_SPACE) {
        *tmp++ = ' ';
    }
    // Extract integer and fractional parts.
    long whole  = (long)num;
    double frac = num - whole;
    // Apply precision to fraction (scale and round).
    frac        = __round(frac * __pow(10, precision));
    // Format integer part
    __format_integer(&tmp, tmp_end, whole, 10, 0, 0, 0);
    // Handle decimal point and fractional part.
    if (precision > 0) {
        __emit_char(&tmp, tmp_end, '.');
        __format_integer(&tmp, tmp_end, (long)frac, 10, precision, precision, 0);
    }
    // Use __format_string to apply padding and copy the formatted float string.
    __format_string(buf, end, float_buf, width, -1, flags);
}

/// @brief Internal function to handle pointer formatting.
/// @param buf Current pointer in the buffer.
/// @param end Pointer to the end of the buffer.
/// @param ptr The pointer to format.
/// @param width The minimum width of the output.
/// @param flags Formatting flags.
static void __format_pointer(char **buf, char *end, void *ptr, int width, int flags)
{
    unsigned long addr = (unsigned long)ptr;
    // Prefix `0x` for pointer formatting.
    __emit_char(buf, end, '0');
    __emit_char(buf, end, 'x');
    __format_unsigned(buf, end, addr, 16, width - 2, 0, 0);
}

/// @brief Handles `%n`, storing the number of characters printed so far.
/// @param count_var Pointer to the integer where the character count is stored.
/// @param count The number of characters written so far.
static void __format_count(int *count_var, int count)
{
    if (count_var) {
        *count_var = count;
    }
}

int fmtlite_vsnprintf(char *buffer, size_t size, const char *format, va_list args)
{
    char dummy;
    char *buf = buffer;
    char *end = buffer + size - 1;

    // Handle NULL buffer case.
    if (!buffer && size == 0) {
        buffer = &dummy;
        buf    = &dummy;
        end    = &dummy;
    }

    // Tracks number of characters written.
    int count = 0;

    while (*format) {
        if (*format == '%') {
            format++; // Skip '%'

            int flags     = 0;
            int width     = 0;
            int precision = -1; // Default: no precision specified
            int length    = 0;  // Length modifier (h, l, ll, etc.)

            // Step 1: Parse Flags.
            while (*format == '-' || *format == '+' || *format == ' ' || *format == '#' || *format == '0') {
                switch (*format) {
                case '-':
                    flags |= FLAGS_LEFT;
                    break;
                case '+':
                    flags |= FLAGS_PLUS;
                    break;
                case ' ':
                    flags |= FLAGS_SPACE;
                    break;
                case '0':
                    flags |= FLAGS_ZEROPAD;
                    break;
                }
                format++;
            }

            // Step 2: Parse Width.
            if (*format == '*') {
                width = va_arg(args, int);
                format++;
            } else {
                while (*format >= '0' && *format <= '9') {
                    width = width * 10 + (*format - '0');
                    format++;
                }
            }

            // Step 3: Parse Precision.
            if (*format == '.') {
                format++;
                if (*format == '*') {
                    precision = va_arg(args, int);
                    format++;
                } else {
                    precision = 0;
                    while (*format >= '0' && *format <= '9') {
                        precision = precision * 10 + (*format - '0');
                        format++;
                    }
                }
                // Precision overrides zero padding.
                if ((precision >= 0) && (flags & FLAGS_ZEROPAD)) {
                    flags &= ~FLAGS_ZEROPAD;
                }
            }

            // Step 4: Parse Length Modifier.
            if (*format == 'h') {
                format++;
                // "hh" (char)
                if (*format == 'h') {
                    length = 2;
                    format++;
                }
                // "h" (short)
                else {
                    length = 1;
                }
            } else if (*format == 'l') {
                format++;
                // "ll" (long)
                if (*format == 'l') {
                    length = 4;
                    format++;
                }
                // "l" (long)
                else {
                    length = 3;
                }
            }

            // Enable uppercase flag if necessary.
            flags |= ((*format == 'X') ? FLAGS_UPPERCASE : 0);

            // Step 5: Parse Specifier and Call Handler.
            switch (*format) {
            case 's': {
                __format_string(&buf, end, va_arg(args, const char *), width, precision, flags);
                break;
            }
            case 'c': {
                __format_char(&buf, end, (char)va_arg(args, int), width, flags);
                break;
            }
            case 'd':
            case 'i': {
                long num;
                if (length == 0) {
                    num = va_arg(args, int);
                } else if (length == 1) {
                    num = (short)va_arg(args, int);
                } else if (length == 2) {
                    num = (char)va_arg(args, int);
                } else {
                    num = va_arg(args, long);
                }
                __format_integer(&buf, end, num, 10, width, precision, flags);
                break;
            }
            case 'u':
            case 'o':
            case 'x':
            case 'X': {
                unsigned long num;
                if (length == 0) {
                    num = va_arg(args, unsigned int);
                } else if (length == 1) {
                    num = (unsigned short)va_arg(args, unsigned int);
                } else if (length == 2) {
                    num = (unsigned char)va_arg(args, unsigned int);
                } else {
                    num = va_arg(args, unsigned long);
                }
                int base;
                if (*format == 'o') {
                    base = 8;
                } else if (*format == 'x' || *format == 'X') {
                    base = 16;
                } else {
                    base = 10;
                }
                __format_unsigned(&buf, end, num, base, width, precision, flags);
                break;
            }
            case 'p': {
                __format_pointer(&buf, end, va_arg(args, void *), width, flags);
                break;
            }
            case 'f':
            case 'F': {
                __format_float(&buf, end, va_arg(args, double), width, precision, flags);
                break;
            }
            case 'n': {
                __format_count(va_arg(args, int *), count);
                break;
            }
            case '%': {
                __emit_char(&buf, end, '%');
                break;
            }
            default: {
                __emit_char(&buf, end, '%');
                __emit_char(&buf, end, *format);
                break;
            }
            }
        } else {
            __emit_char(&buf, end, *format);
        }

        format++;
        count++;
    }
    if (buffer)
        *buf = '\0';
    return buf - buffer;
}

int fmtlite_snprintf(char *str, size_t size, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int result = fmtlite_vsnprintf(str, size, format, args);
    va_end(args);
    return result;
}

int fmtlite_sprintf(char *str, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int result = fmtlite_vsnprintf(str, 4096, format, args);
    va_end(args);
    return result;
}
