#include <stdio.h>
#include <string.h>

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

int roman_to_dec(const char *str) {
    size_t len = strlen(str);

    unsigned curr_num_occurances = 1;
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
            total -= curr_num_occurances * 2 * prev_val;
            curr_num_occurances = 1;

        } else if (prev_val > value) {
            //      v
            // Ex. VI, previous value greater than current value,
            // reset number of occurances of current value.
            curr_num_occurances = 1;

        } else {
            //      v
            // Ex. VV, values are the same, increment occurances.
            ++curr_num_occurances;
        }

        prev_val = value;
    }

    return total;
}
