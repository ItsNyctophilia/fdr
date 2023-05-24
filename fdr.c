// Code taken from class examples, by professor Noel
#include "utils/fdr_utils.h"
#include <stdio.h>
#include <sys/types.h>
#include <sysexits.h> // exit codes
#include <unistd.h>

enum { PORT_STR_LEN = 6, NUM_PORTS = 3 };

int main(int argc, char *argv[]) {
    // TODO: add support for command line arguments
    if (argc != 1) {
        fprintf(stderr, "Usage: %s \n", argv[0]);
        return EX_USAGE;
    }

    begin();

    // open and bind ports
    uid_t uid = getuid();
    int sockets[NUM_PORTS] = {0};
    int8_t valid_sockets = 0;
    for (size_t i = 0; i < NUM_PORTS; i++) {
        char port_str[PORT_STR_LEN];
        if (!port_to_str(uid, i, port_str, PORT_STR_LEN)) {
            fprintf(stderr, "Invalid port: %s\n", port_str);
            continue;
        }

        sockets[i] = prepare_socket(port_str);
        if (sockets[i] < 0) {
            fprintf(stderr,
                    "Error binding sockets; trying to shutdown cleanly...\n");
            for (int j = (int)i - 1; j >= 0; j--) {
                fprintf(stderr, "Closing socket %d\n", sockets[j]);
                close(sockets[j]);
            }
            return -sockets[i]; // flip the sign back to the original error code
        }
        valid_sockets++;
    }
    if (valid_sockets == 0) {
        fprintf(stderr, "No valid sockets opened\n");
        return EX_UNAVAILABLE;
    }

    // TODO: open threads for each port
    // wait until a request to shutdown the server is made
    end(sockets, NUM_PORTS);
}
