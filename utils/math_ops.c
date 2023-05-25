#include "math_ops.h"
#include "dec_to_hex.h"
#include "roman_to_hex.h"
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

enum { MAX_FIB = 300 };

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
    char *endptr;
    long step = strtol(input, &endptr, 10);
    // check if input is a non-number or is outside of allowed values
    if (*endptr || step > MAX_FIB || step < 0) {
        return EX_USAGE;
    }

    char temp[BUF_LEN] = {0};
    fibonacci(step, temp, BUF_LEN);
    size_t non_zero = strspn(temp, "0");
    if (non_zero == strlen(temp)) {
        // check if output is 0
        non_zero--;
    }
    strncpy(output, temp + non_zero, output_len);
    return EX_OK;
}
