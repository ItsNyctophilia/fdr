#include "math_ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
enum { MAX_FIB = 300 };

/* STATIC FUNCTIONS */
static int roman_value(char letter) {
    switch (letter) {
    case 'I':
        return 1;
    case 'V':
        return 5;
    case 'X':
        return 10;
    case 'L':
        return 50;
    case 'C':
        return 100;
    case 'D':
        return 500;
    case 'M':
        return 1000;
    default:
        // Error case, safe to be 0, because
        // there is no zero in roman numerals
        return 0;
    }
}

static int roman_to_dec(const char *str) {
    size_t len = strlen(str);

    unsigned curr_num_occurrences = 1;
    int total = 0;
    int prev_val = 0;

    for (size_t i = 0; i < len; ++i) {

        int value = roman_value(str[i]);

        if (value == 0) {
            // Occurs if the character was not a roman numeral
            return value;
        }
        total += value;

        if (!prev_val) {
            // Occurs for the first numeral in sequence only
            prev_val = value;
            continue;
        }

        if (prev_val < value) {
            //      v
            // Ex. IV, previous value less than current value,
            // so perform subtraction and undo previous addition.
            total -= curr_num_occurrences * 2 * prev_val;
            curr_num_occurrences = 1;

        } else if (prev_val > value) {
            //      v
            // Ex. VI, previous value greater than current value,
            // reset number of occurrences of current value.
            curr_num_occurrences = 1;

        } else {
            //      v
            // Ex. VV, values are the same, increment occurrences.
            ++curr_num_occurrences;
        }

        prev_val = value;
    }

    return total;
}

static void add_dec_value(char *hex_array, size_t num_elements, char value) {
    unsigned char carry = value;
    for (int i = num_elements - 1; i >= 0; --i) {
        unsigned char tmp = (hex_array[i] * 10) + carry;
        hex_array[i] = tmp % 16;
        carry = tmp / 16;
    }
}

static char *init_hex_array(const char *dec_str, size_t *num_elements) {
    if (!dec_str || !num_elements) {
        return NULL;
    }
    *num_elements = strlen(dec_str);
    char *hex_array = calloc(*num_elements, sizeof(*hex_array));

    for (size_t i = 0; i < *num_elements; ++i) {
        // Convert ASCII integers to raw values and add to array
        add_dec_value(hex_array, *num_elements, dec_str[i] - '0');
    }

    // Returned char * must be freed by caller
    return hex_array;
}

static char *get_hex_array(char *hex_array, size_t num_elements) {
    if (!hex_array) {
        return NULL;
    }
    const char hex_chars[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    size_t offset = 0;

    while ((hex_array[offset] == 0) && (offset < num_elements - 1)) {
        // Skip past leading zeroes
        ++offset;
    }
    size_t counter = 0;
    // Constant 1 is terminating '\0'
    char *hex_str = calloc(num_elements + 1, sizeof(*hex_str));
    for (size_t i = offset; i < num_elements; ++i) {
        hex_str[counter] = hex_chars[hex_array[i]];
        ++counter;
    }
    // Append null terminator
    hex_str[num_elements] = '\0';
    return hex_str;
}
static int fibonacci(long step, const char *output, size_t output_len);

/* PUBLIC FUNCTIONS */
int roman_to_hex(const char *input, char *output, size_t output_len) {
    int converted = roman_to_dec(input);
    snprintf(output, output_len, "0x%x", converted);
    return 0;
}

int dec_to_hex(const char *input, char *output, size_t output_len) {
    size_t input_len = strlen(input);
    char *converted = init_hex_array(input, &input_len);

    char *formatted = get_hex_array(converted, input_len);
    snprintf(output, output_len, "0x%s", formatted);
    // strncpy(output, formatted, output_len);
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
    snprintf(output, output_len, "0x%s", temp + non_zero);
    // strncpy(output, temp + non_zero, output_len);
    return EX_OK;
}
