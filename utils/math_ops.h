#ifndef MATH_OPS_H
#define MATH_OPS_H

#include <stdbool.h>
#include <stdlib.h>

enum { BUF_LEN = 1024 };
/**
 * @brief Convert a string of Roman numerals to a hexadecimal number.
 *
 * @param input The string of Roman numerals to convert.
 * @param output Where to store the output.
 * @param output_len The initial size of the output buffer.
 * @param uppercase If the hex output should be in uppercase
 * @return int Status of conversion.
 */
int roman_to_hex(const char *input, char *output, size_t output_len,
		 bool uppercase);

/**
 * @brief Convert a string of decimal numbers to a hexadecimal number.
 *
 * @param input The string of decimal numbers to convert.
 * @param output Where to store the output.
 * @param output_len The initial size of the output buffer.
 * @param uppercase If the hex output should be in uppercase
 * @param max_len The maximum allowed characters in the input string.
 * @return int Status of conversion.
 */
int dec_to_hex(const char *input, char *output, size_t output_len,
	       bool uppercase, size_t max_len);

/**
 * @brief Calculate the Fibonacci sequence.
 *
 *  Calculate the Fibonacci up to the decimal number represented in input,
 * and store the result as hexadecimal in output.
 *
 * @param input The string of decimal numbers to calculate to.
 * @param output Where to store the output.
 * @param output_len The initial size of the output buffer.
 * @param uppercase If the hex output should be in uppercase
 * @return int Status of calculation.
 */
int fib_to_hex(const char *input, char *output, size_t output_len,
	       bool uppercase);

#endif				/* MATH_OPS_H */
