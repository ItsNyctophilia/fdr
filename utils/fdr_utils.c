// Code taken from class examples, by professor Noel
#define _POSIX_C_SOURCE 201112L
#include "fdr_utils.h"
#include <netdb.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sysexits.h> // exit codes
#include <unistd.h>

enum { PORT_OFFSET = 1000, VALID_PORT = 1024, BUF_LEN = 1024 };
static sem_t shutdown_semaphore;

/* STATIC FUNCTIONS */
static void serve_port(int sd) {
    for (;;) {
        struct sockaddr_storage client;
        socklen_t client_sz = sizeof(client);

        char buffer[1024];
        // TODO: add logging information using syslog(3)
        ssize_t received = recvfrom(sd, buffer, sizeof(buffer) - 1, 0,
                                    (struct sockaddr *)&client, &client_sz);
        if (received < 0) {
            fprintf(stderr, "Closing socket %d: ", sd);
            perror("Unable to receive");
            close(sd);
            return;
        }
        buffer[received] = '\0';
        printf("Message: %s\n", buffer);
    }
}

/* PUBLIC FUNCTIONS */
bool port_to_str(u_int32_t base, size_t scale, char *port_str, size_t len) {
    u_int32_t port = base + PORT_OFFSET * scale;
    snprintf(port_str, len, "%d", port);
    return port < VALID_PORT ? false : true;
}

int prepare_socket(const char *port_str) {
    struct addrinfo hints = {0};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    struct addrinfo *results;

    int err = getaddrinfo(NULL, port_str, &hints, &results);
    if (err != 0) {
        fprintf(stderr, "Cannot get address: %s\n", gai_strerror(err));
        return -EX_NOHOST;
    }

    int sd =
        socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if (sd < 0) {
        perror("Could not create socket");
        freeaddrinfo(results);
        return -EX_OSERR;
    }

    err = bind(sd, results->ai_addr, results->ai_addrlen);
    if (err < 0) {
        perror("Could not bind socket");
        close(sd);
        freeaddrinfo(results);
        return -EX_OSERR;
    }
    freeaddrinfo(results);

    return sd;
}

void shutdown_handler(int signum) { sem_post(&shutdown_semaphore); }

void begin(void) {
    sem_init(&shutdown_semaphore, 0, 0);
    const struct sigaction shutdown_action = {.sa_handler = shutdown_handler};
    // TODO: handle other possibly program ending signals
    sigaction(SIGINT, &shutdown_action, NULL);
}

int end(int *sockets, size_t sock_len) {
    sem_wait(&shutdown_semaphore);
    // TODO: gracefully close threads
    for (int i = 0; i < sock_len; i++) {
        if (sockets[i] > 2) { // don't accidentally close stdin, stdout, stderr
            close(sockets[i]);
        }
    }
    return EX_OK;
}

void *service_thread(void *arg) {
    sigset_t old_set, merge_set;
    sigemptyset(&merge_set);
    sigaddset(&merge_set, SIGINT);
    pthread_sigmask(SIG_BLOCK, &merge_set, &old_set);

    int sd = *((int *)arg);
    serve_port(sd);

    pthread_sigmask(SIG_SETMASK, &old_set, NULL);

    return NULL;
}
