#include "math_ops.h"
#include <stdio.h> // WARNING: TESTING ONLY

int roman_to_hex(const char *input, char *output, size_t output_len) {
    snprintf(output, output_len, "Roman Numeral: %s\n", input);
    return 0;
}

int dec_to_hex(const char *input, char *output, size_t output_len) {
    snprintf(output, output_len, "Large Decimal: %s\n", input);
    return 0;
}

int fib_to_hex(const char *input, char *output, size_t output_len) {
    snprintf(output, output_len, "Fibonacci: %s\n", input);
    return 0;
}

void printable_hex_array(const char *hex_array, char *output,
                         size_t output_len) {
    snprintf(output, output_len, "%s", hex_array);
}
