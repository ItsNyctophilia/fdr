#include "math_ops.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
enum { MAX_FIB = 300 };

/* STATIC FUNCTIONS */
static void lower_str(char *str) {
    for (size_t i = 0; str[i]; ++i) {
        str[i] = tolower(str[i]);
    }
}
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

static bool validate_roman(const char *input) {
    // limit to 4 numerals, reject non roman numerals
    static char *roman_alphabet = "IVXLCDM";
    size_t in_len = strlen(input);
    if (in_len > 4) {
        return false;
    }
    for (size_t i = 0; i < in_len; i++) {
        if (!strchr(roman_alphabet, input[i])) {
            return false;
        }
    }
    return true;
}

static bool validate_decimal(const char *input, size_t max_len) {
    size_t in_len = strlen(input);
    if (in_len > max_len) {
        return false;
    }
    int zero_count = 0;
    bool leading_one = false;
    for (size_t i = 0; i < in_len; i++) {
        if (i == 0 && input[i] == '1') {
            leading_one = true;
        }
        if (!isdigit(input[i])) {
            return false;
        }
        if (input[i] == '0') {
            ++zero_count;
        }
    }
    // check for input higher than 10^MAX_LEN
    if (in_len == max_len && leading_one && zero_count > max_len - 1) {
        return false;
    }

    return true;
}

static int fibonacci(long step, const char *output, size_t output_len);

/* PUBLIC FUNCTIONS */
int roman_to_hex(const char *input, char *output, size_t output_len,
                 bool uppercase) {
    if (!validate_roman(input)) {
        return EX_USAGE;
    }
    int converted = roman_to_dec(input);
    if (uppercase) {
        snprintf(output, output_len, "0x%X", converted);
    } else {
        snprintf(output, output_len, "0x%x", converted);
    }
    return EX_OK;
}

int dec_to_hex(const char *input, char *output, size_t output_len,
               bool uppercase, size_t max_len) {
    if (!validate_decimal(input, max_len)) {
        return EX_USAGE;
    }
    size_t input_len = strlen(input);
    char *converted = init_hex_array(input, &input_len);

    char *formatted = get_hex_array(converted, input_len);
    if (!uppercase) {
        lower_str(formatted);
    }
    snprintf(output, output_len, "0x%s", formatted);
    // strncpy(output, formatted, output_len);
    free(formatted);
    free(converted);
    return EX_OK;
}

int fib_to_hex(const char *input, char *output, size_t output_len,
               bool uppercase) {
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
    if (!uppercase) {
        lower_str(temp);
    }
    snprintf(output, output_len, "0x%s", temp + non_zero);
    // strncpy(output, temp + non_zero, output_len);
    return EX_OK;
}
