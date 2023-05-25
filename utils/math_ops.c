#include "math_ops.h"
#include "dec_to_hex.h"
#include "roman_to_hex.h"
#include <stdio.h>
#include <stdlib.h>

/* STATIC FUNCTIONS */
static int fibonacci(long step, const char *output, size_t output_len);

/* PUBLIC FUNCTIONS */
int roman_to_hex(const char *input, char *output, size_t output_len) {
    int converted = roman_to_dec(input);
    snprintf(output, output_len, "%x\n", converted);
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
    // TODO: error check input
    long step = strtol(input, NULL, 10);
    fibonacci(step, output, output_len);
    return 0;
}
