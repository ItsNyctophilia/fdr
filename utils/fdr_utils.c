// Code taken from class examples, by professor Noel
#define _POSIX_C_SOURCE 201112L
#include "fdr_utils.h"
#include "math_ops.h"
#include <arpa/inet.h>
#include <ctype.h>
#include <getopt.h>
#include <netdb.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sysexits.h> // exit codes
#include <syslog.h>
#include <unistd.h>

enum {
    PORT_OFFSET = 1000,
    VALID_PORT = 1024,
    LOG_LEVEL = LOG_INFO | LOG_USER,
    ERROR_LEN = BUF_LEN * 2,
    MAX_DEC_LEN = 19
};
static sem_t shutdown_semaphore;
struct client_info {
    char addr[INET6_ADDRSTRLEN];
    uint16_t port;
};

bool case_matching = false;
bool send_error = false;

/* STATIC FUNCTIONS */
static int process_args(int *argc, char **argv[]) {
    int opt;

    const char short_opts[] = ":ie";

    while ((opt = getopt(*argc, *argv, short_opts)) != -1) {
        switch (opt) {
        case 'i':
            // i[nsensitive]
            case_matching = true;
            break;
        case 'e':
            // e[rror]
            send_error = true;
            break;
        case '?':
            // unrecognized option
            return EX_USAGE;
        }
    }

    // readjust argc and argv
    *argc -= optind;
    *argv += optind;
    return EX_OK;
}
static void probe_client(const struct sockaddr *client,
                         struct client_info *info) {
    // get information from the client
    if (client->sa_family == AF_INET6) {
        inet_ntop(client->sa_family,
                  &((struct sockaddr_in6 *)client)->sin6_addr, info->addr,
                  sizeof(info->addr));
        info->port = ntohs(((struct sockaddr_in6 *)client)->sin6_port);
    } else {
        inet_ntop(client->sa_family, &((struct sockaddr_in *)client)->sin_addr,
                  info->addr, sizeof(info->addr));
        info->port = ntohs(((struct sockaddr_in *)client)->sin_port);
    }
}

static void log_request(const struct sockaddr *client, int sd,
                        const char *request) {
    struct client_info info = {0};
    probe_client(client, &info);
    syslog(LOG_LEVEL,
           "Receive connection from %s:%hu on socket %d with request %s",
           info.addr, info.port, sd, request);
}

static void log_error(const struct sockaddr *client, socklen_t client_sz,
                      int sd, const char *msg, const char *input) {
    syslog(LOG_LEVEL, "%s: %s", msg, input);
    if (send_error) {
        char err_msg[ERROR_LEN] = {0};
        snprintf(err_msg, sizeof(err_msg), "%s: %s\n", msg, input);
        sendto(sd, err_msg, strlen(err_msg), 0, client, client_sz);
    }
}

static void log_response(const struct sockaddr *client, int sd,
                         const char *input, const char *output) {
    struct client_info info = {0};
    probe_client(client, &info);
    syslog(LOG_LEVEL, "Sending response (%s) to client %s:%hu on socket %d",
           output, info.addr, info.port, sd);
}

static void serve_port(int sd) {
    int err;
    for (;;) {
        bool uppercase = false;
        struct sockaddr_storage client;
        socklen_t client_sz = sizeof(client);

        char input[1024];
        ssize_t received = recvfrom(sd, input, sizeof(input) - 1, 0,
                                    (struct sockaddr *)&client, &client_sz);
        if (received < 0) {
            fprintf(stderr, "Closing socket %d: ", sd);
            perror("Unable to receive");
            close(sd);
            return;
        }
        log_request((const struct sockaddr *)&client, sd, input);

        char response[BUF_LEN] = {0};
        char working_response[BUF_LEN] = {0};
        input[received] = '\0';

        int operation = input[0];
        switch (toupper(operation)) {
        // TODO: send a response with -e flag
        case 'F':
            if (case_matching && operation == 'F') {
                uppercase = true;
            }
            err = fib_to_hex(input + 1, response, BUF_LEN, uppercase);
            if (err) {
                log_error((const struct sockaddr *)&client, client_sz, sd,
                          "Invalid input", input);
                continue;
            }
            break;
        case 'D':
            if (case_matching && operation == 'D') {
                uppercase = true;
            }
            err = dec_to_hex(input + 1, response, BUF_LEN, uppercase,
                             MAX_DEC_LEN);
            if (err) {
                log_error((const struct sockaddr *)&client, client_sz, sd,
                          "Invalid input", input);
                continue;
            }
            break;
        case 'R':
            if (case_matching && operation == 'R') {
                uppercase = true;
            }
            err = roman_to_hex(input + 1, response, BUF_LEN, uppercase);
            if (err) {
                log_error((const struct sockaddr *)&client, client_sz, sd,
                          "Invalid input", input);
                continue;
            }
            break;
        default:
            log_error((const struct sockaddr *)&client, client_sz, sd,
                      "Error reading operation code", input);
            // drop input and get ready for more input without sending response
            continue;
            break;
        }
        sendto(sd, response, strlen(response), 0, (struct sockaddr *)&client,
               client_sz);
        log_response((const struct sockaddr *)&client, sd, input, response);
    }
}

static void shutdown_handler(int signum) { sem_post(&shutdown_semaphore); }

/* PUBLIC FUNCTIONS */
bool port_to_str(u_int32_t base, size_t scale, char *port_str, size_t len) {
    u_int32_t port = base + PORT_OFFSET * scale;
    snprintf(port_str, len, "%d", port);
    return port < VALID_PORT ? false : true;
}

int prepare_socket(const char *port_str) {
    struct addrinfo hints = {0};
    hints.ai_family = AF_UNSPEC;
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

int begin(int *argc, char **argv[]) {
    int err = process_args(argc, argv);
    if (err) {
        return err;
    }
    // open syslog for logging
    openlog(*argv[0], LOG_PID | LOG_PERROR, LOG_USER);
    sem_init(&shutdown_semaphore, 0, 0);
    const struct sigaction shutdown_action = {.sa_handler = shutdown_handler};
    sigaction(SIGINT, &shutdown_action, NULL);
    return EX_OK;
}

int end(int *sockets, pthread_t *threads, size_t sock_len) {
    // wait until a shutdown signal is sent
    sem_wait(&shutdown_semaphore);
    for (int i = 0; i < sock_len; i++) {
        if (sockets[i] > 2) { // don't accidentally close stdin, stdout, stderr
            void *unused;
            pthread_cancel(threads[i]);
            pthread_join(threads[i], &unused);
            close(sockets[i]);
        }
    }
    // close syslog
    closelog();
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
