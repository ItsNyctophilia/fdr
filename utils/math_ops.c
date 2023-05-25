#include "math_ops.h"
#include "dec_to_hex.h"
#include <stdio.h> // WARNING: TESTING ONLY

int roman_to_hex(const char *input, char *output, size_t output_len) {
    snprintf(output, output_len, "Roman Numeral: %s\n", input);
    return 0;
}

int dec_to_hex(const char *input, char *output, size_t output_len) {
    size_t input_len = strlen(input);
    char *converted = init_hex_array(input, &input_len);

    char *formatted = get_hex_array(converted, input_len);
    strncpy(output, formatted, output_len);
    free(formatted);
    free(converted);
    return 0;
}

int fib_to_hex(const char *input, char *output, size_t output_len) {
    snprintf(output, output_len, "Fibonacci: %s\n", input);
    return 0;
}
