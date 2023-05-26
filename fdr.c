// Code taken from class examples, by professor Noel
#include "utils/fdr_utils.h"
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sysexits.h> // exit codes
#include <unistd.h>

enum { PORT_STR_LEN = 6, NUM_PORTS = 3 };

int main(int argc, char *argv[]) {
	char *prog_name = argv[0];
	int err = begin(&argc, &argv);
	if (err || argc > 0) {
		fprintf(stderr, "Usage: %s [-d] [-i] [-e]\n", prog_name);
		return err;
	}

	// open and bind ports
	uid_t uid = getuid();
	int sockets[NUM_PORTS] = {0};
	bool valid_sockets = false;
	for (size_t i = 0; i < NUM_PORTS; i++) {
		char port_str[PORT_STR_LEN];
		if (!port_to_str(uid, i, port_str, PORT_STR_LEN)) {
			fprintf(stderr, "Invalid port: %s\n", port_str);
			continue;
		}

		sockets[i] = prepare_socket(port_str);
		if (sockets[i] < 0) {
			fprintf(stderr, "Error binding sockets; trying to "
					"shutdown cleanly...\n");
			for (int j = (int)i - 1; j >= 0; j--) {
				fprintf(stderr, "Closing socket %d\n",
					sockets[j]);
				close(sockets[j]);
			}
			// flip the sign back to the original error code
			return -sockets[i];
		}
		valid_sockets = true;
	}
	if (!valid_sockets) {
		fprintf(stderr, "No valid sockets opened\n");
		return EX_UNAVAILABLE;
	}

	pthread_t threads[NUM_PORTS] = {0};

	for (int i = 0; i < NUM_PORTS; i++) {
		if (sockets[i]) { // skip sockets that were not opened
			pthread_create(&threads[i], NULL, service_thread,
				       &sockets[i]);
		}
	}
	// wait until a request to shutdown the server is made
	return end(sockets, threads, NUM_PORTS);
}
