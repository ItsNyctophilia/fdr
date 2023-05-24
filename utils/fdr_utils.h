#include <stdbool.h>
#include <sys/types.h>

/**
 * @brief Open and bind a socket on the given port number for listening.
 *
 * @param port The port to open the socket on
 * @return int The opened socket descriptor, or a negative number for error.
 */
int prepare_socket(const char *port_str);

/**
 * @brief Convert a number to a string of a port, based on set offset.
 *
 * The offset is 1000. If the adjusted port number is less than 1024, this
 * function will return false, but the number will always be converted
 *
 * @param base The base number to convert to a port string.
 * @param scale The scale to multiply the offset by.
 * @param port_str Where to store the port string.
 * @param len The length of the port_str buffer.
 * @return true If the adjusted number is a valid port.
 * @return false If the adjusted number is not a valid port.
 */
bool port_to_str(u_int32_t base, size_t scale, char *port_str, size_t len);
