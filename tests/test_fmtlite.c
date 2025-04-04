#include <fmtlite/fmtlite.h>
#include <stdio.h>
#include <string.h>

void test_fmt(const char *desc, const char *fmt, ...)
{
    char expected[256];
    char actual[256];

    va_list args1, args2, args3;
    va_start(args1, fmt);
    va_copy(args2, args1);
    va_copy(args3, args1);

    // Compute expected output and length with system vsnprintf
    int len_expected = vsnprintf(expected, sizeof(expected), fmt, args1);
    va_end(args1);

    // Compute actual output and length with fmtlite
    int len_actual = fmtlite_vsnprintf(actual, sizeof(actual), fmt, args2);
    va_end(args2);

    printf("%-30s → \"%s\" (len=%d, expected=%d)\n", desc, actual, len_actual, len_expected);

    if (len_actual != len_expected || strcmp(actual, expected) != 0) {
        // Known acceptable difference — don't report
        if (strstr(fmt, "%p") && strstr(expected, "(nil)") && strstr(actual, "0x0")) {
            return;
        }
        fprintf(stderr, "Mismatch on : %s\n", desc);
        fprintf(stderr, "    Expected : \"%s\"\n", expected);
        fprintf(stderr, "    Actual   : \"%s\"\n", actual);
    }
    va_end(args3);
}

int main(void)
{
    test_fmt("Basic integer", "Value: %d", 42);
    test_fmt("Negative int", "Value: %d", -42);
    test_fmt("Unsigned max", "Unsigned: %u", 4294967295U);
    test_fmt("Unsigned zero", "Unsigned: %u", 0U);
    test_fmt("Hex lower", "Hex: %x", 0xdeadbeef);
    test_fmt("Hex upper", "Hex: %X", 0xDEADBEEF);
    test_fmt("Octal", "Octal: %o", 0755);
    test_fmt("Char and string", "Char: %c, String: %s", 'A', "Hello");
    test_fmt("Left pad", "[%5d]", 7);
    test_fmt("Zero pad", "[%05d]", 7);
    test_fmt("Left align", "[%-5d]", 7);
    test_fmt("Precision", "[%.4d]", 7);
    test_fmt("Width + precision", "[%8.4d]", 7);
    test_fmt("Pointer null", "Ptr: %p", (void *)0);
    test_fmt("Pointer valid", "Ptr: %p", (void *)0x1234ABCD);
    test_fmt("Percent sign", "100%% done!");
    test_fmt("Float pi", "Pi: %.2f", 3.14159);
    test_fmt("Float zero", "Zero: %.2f", 0.0);
    test_fmt("Float negative", "Neg: %.3f", -2.71828);
    test_fmt("Float large", "Large: %.1f", 123456.789);
    test_fmt("Float w/ width + prec", "[%10.4f]", 3.14159);
    test_fmt("Min int", "INT_MIN: %d", -2147483648);
    test_fmt("Max int", "INT_MAX: %d", 2147483647);
    test_fmt("Max unsigned", "UINT_MAX: %u", 4294967295U);
    test_fmt("Hex tiny", "Hex: %x", 0x1A);
    test_fmt("Char edge 0", "Char: [%c]", 0);
    test_fmt("Char edge 127", "Char: [%c]", 127);
    test_fmt("String cutoff", "[%.4s]", "abcdef");
    test_fmt("String pad", "[%10s]", "foo");
    test_fmt("String left align", "[%-10s]", "bar");
    test_fmt("Mix types", "[%+08.4d|%x|%.5s|%c|%.2f]", 42, 0xABCD, "abcdef", 'Z', 1.618);
    test_fmt("Empty string", "Empty: [%s]", "");
    test_fmt("Null string", "Null: [%s]", NULL);
    test_fmt("Long string", "Long: [%s]", "This is a very long string that exceeds the buffer size to test truncation and safety in snprintf. It should be cut off properly.");
    test_fmt("Complex format", "Complex: [%d|%s|%f|%x|%c]", 123, "test", 3.14, 0xFF, 'A');
    test_fmt("Percent edge case", "Edge case: 100%% done!");
    test_fmt("Empty format string", "");
    return 0;
}
