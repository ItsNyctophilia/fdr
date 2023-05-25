#ifndef DEC_TO_HEX_H
#define DEC_TO_HEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *init_hex_array(const char *dec_str, size_t *num_elements);

char *get_hex_array(const char *hex_array, size_t num_elements);

#endif
