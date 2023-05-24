#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void add_dec_value(char *hex_array, size_t num_elements, char value)
{
	unsigned char carry = value;
	for (int i = num_elements - 1; i >= 0; --i) {
		unsigned char tmp = (hex_array[i] * 10) + carry;
		hex_array[i] = tmp % 16;
		carry = tmp / 16;
	}
}

char *init_hex_array(char *dec_str, size_t *num_elements)
{
	*num_elements = strlen(dec_str);
	char *hex_array = calloc(*num_elements, sizeof(*hex_array));

	for (size_t i = 0; i < *num_elements; ++i) {
	    // Convert ASCII integers to raw values and add to array
		add_dec_value(hex_array, *num_elements, dec_str[i] - '0');
	}
	
	// Returned char * must be freed by caller
	return hex_array;
}

