// sourced from user hajikelist at source
// https://stackoverflow.com/a/861084
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void add_dec_value(char *hex_array, size_t num_elements, char value) {
    unsigned char carry = value;
    for (int i = num_elements - 1; i >= 0; --i) {
        unsigned char tmp = (hex_array[i] * 10) + carry;
        hex_array[i] = tmp % 16;
        carry = tmp / 16;
    }
}

char *init_hex_array(const char *dec_str, size_t *num_elements) {
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

char *get_hex_array(char *hex_array, size_t num_elements) {
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
