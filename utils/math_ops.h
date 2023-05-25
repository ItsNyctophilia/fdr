#ifndef MATH_OPS_H
#define MATH_OPS_H

#include <stdlib.h>

enum { BUF_LEN = 1024 };
/**
 * @brief Convert a string of Roman numerals to a hexadecimal number.
 *
 * @param input The string of Roman numerals to convert.
 * @param output Where to store the output.
 * @param output_len The initial size of the output buffer.
 * @return int Status of conversion.
 */
int roman_to_hex(const char *input, char *output, size_t output_len);

/**
 * @brief Convert a string of decimal numbers to a hexadecimal number.
 *
 * @param input The string of decimal numbers to convert.
 * @param output Where to store the output.
 * @param output_len The initial size of the output buffer.
 * @return int Status of conversion.
 */
int dec_to_hex(const char *input, char *output, size_t output_len);

/**
 * @brief Calculate the Fibonacci sequence.
 *
 *  Calculate the Fibonacci up to the decimal number represented in input,
 * and store the result as hexadecimal in output.
 *
 * @param input The string of decimal numbers to calculate to.
 * @param output Where to store the output.
 * @param output_len The initial size of the output buffer.
 * @return int Status of calculation.
 */
int fib_to_hex(const char *input, char *output, size_t output_len);

/**
 * @brief Prepare a hex array to be printable by removing leading 0's.
 *
 * @param hex_array The array to prepare.
 * @param output Where to store the output.
 * @param output_len The initial size of the output buffer.
 */
void printable_hex_array(const char *hex_array, char *output,
                         size_t output_len);

#endif /* MATH_OPS_H */
