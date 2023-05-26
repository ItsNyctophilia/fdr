#ifndef FDR_UTILS_H
#define FDR_UTILS_H
#include <pthread.h>
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

/**
 * @brief Start the server.
 *
 * Will initialize any needed values on the back end.
 *
 * @param argc The amount of program arguments
 * @param argv The program arguments
 *
 */
int begin(int *argc, char **argv[]);

/**
 * @brief Wait for the server to finish, then clean up resources.
 *
 * @param sockets The opened sockets to be closed.
 * @param threads The spawned threads.
 * @param sock_len The number of opened sockets.
 * @return int 0 on successful server close, non-zero on error.
 */
int end(int *sockets, pthread_t *threads, size_t sock_len);

void *service_thread(void *arg);

#endif /* FDR_UTILS_H */
