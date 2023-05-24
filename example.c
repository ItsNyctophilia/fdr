// WARNING: this code is only for testing/sandbox purposes
// Code taken from class examples, by professor Noel
#define _POSIX_C_SOURCE 201112L
#define _DEFAULT_SOURCE

#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <stdlib.h> // exit

#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

// In the library, but not the headers on the test system
extern pid_t gettid(void);

// return value: the negative of an appropriate error code, or a valid socket fd
static int prepare_socket(const char *port_string) {
    struct addrinfo hints = {0};
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    struct addrinfo *results;
    int err = getaddrinfo(NULL, port_string, &hints, &results);
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

typedef struct {
    const struct sockaddr *client;
    socklen_t client_sz;
    char *input;
    char *output;
    size_t input_len;
    size_t output_len;
} request_t;

static void print_connection_message(const struct sockaddr *client) {
    char addr[INET6_ADDRSTRLEN];
    unsigned short port = 0;

    if (client->sa_family == AF_INET6) {
        inet_ntop(client->sa_family,
                  &((struct sockaddr_in6 *)client)->sin6_addr, addr,
                  sizeof(addr));
        port = ntohs(((struct sockaddr_in6 *)client)->sin6_port);
    } else {
        inet_ntop(client->sa_family, &((struct sockaddr_in *)client)->sin_addr,
                  addr, sizeof(addr));
        port = ntohs(((struct sockaddr_in *)client)->sin_port);
    }

    // pthread_self(3) is the POSIX thread ID, but is not related to any process
    // ID, whereas gettid(2) is Linux-specific, but the result matches the
    // kernel's thread ID.
    printf("Thread %ld (%d) received from %s:%hu\n", pthread_self(), gettid(),
           addr, port);
}

static void process_request(request_t *request) {
    print_connection_message(request->client);

    request->input[request->input_len] = '\0';

    printf("%s\n\n", request->input);

    if (request->output_len < strlen("ACK") + 1) {
        request->output = realloc(request->output, strlen("ACK") + 1);
    }

    if (request->output) {
        strcpy(request->output, "ACK");
        request->output_len = strlen("ACK") + 1;
    }
}

static void serve_port(int sd) {
    for (;;) {
        struct sockaddr_storage client;
        socklen_t client_sz = sizeof(client);

        // TODO: choose a more appropriate value for buffer length
        // Internet minimum dgram size is 576, so round down to
        // nearest power of 2
        char buffer[512];
        // TODO: add logging information using syslog(3)
        ssize_t received = recvfrom(sd, buffer, sizeof(buffer) - 1, 0,
                                    (struct sockaddr *)&client, &client_sz);
        if (received < 0) {
            fprintf(stderr, "Closing socket %d: ", sd);
            perror("Unable to receive");
            close(sd);
            return;
        }

        request_t request = {.client = (struct sockaddr *)&client,
                             .client_sz = client_sz,
                             .input = buffer,
                             .input_len = (size_t)received,
                             .output = NULL,
                             .output_len = 0};

        // TODO: switch case to choose operation based on input from client
        process_request(&request);

        if (request.output_len) {
            sendto(sd, request.output, request.output_len, 0,
                   (struct sockaddr *)&client, client_sz);
            free(request.output);
        }
    }
}

static void acknowledge_cancel(void *arg) {
    (void)arg;

    printf("Thread %ld (%d) received cancellation signal.\n", pthread_self(),
           gettid());
}

static void *service_thread(void *arg) {
    pthread_cleanup_push(acknowledge_cancel, NULL);

    sigset_t old_set, merge_set;
    sigemptyset(&merge_set);
    sigaddset(&merge_set, SIGINT);
    pthread_sigmask(SIG_BLOCK, &merge_set, &old_set);

    int sd = *((int *)arg);
    serve_port(sd);

    pthread_sigmask(SIG_SETMASK, &old_set, NULL);

    pthread_cleanup_pop(0);

    return NULL;
}

static sem_t shutdown_semaphore;

static void shutdown_handler(int signum) {
    fprintf(stdout, "Thread %d handled signal %d\n", gettid(),
            signum); // POC only!
    sem_post(&shutdown_semaphore);
}

// TODO: don't hard code port numbers, get from uid
enum {
    NUM_PORTS = 3,
    PORT_STR_LEN = 5,
    PORT_1 = 5000,
    PORT_2 = 5001,
    PORT_3 = 5002,
};

int main(int argc, char *argv[]) {
    // TODO: add support for command line arguments
    if (argc != 1) {
        fprintf(stderr, "Usage: %s \n", argv[0]);
        return EX_USAGE;
    }

    sem_init(&shutdown_semaphore, 0, 0);
    const struct sigaction shutdown_action = {.sa_handler = shutdown_handler};
    sigaction(SIGINT, &shutdown_action, NULL);

    int sockets[NUM_PORTS] = {0};

    char port[PORT_STR_LEN];
    snprintf(port, PORT_STR_LEN, "%d", PORT_1);
    sockets[0] = prepare_socket(port);
    snprintf(port, PORT_STR_LEN, "%d", PORT_2);
    sockets[1] = prepare_socket(port);
    snprintf(port, PORT_STR_LEN, "%d", PORT_3);
    sockets[2] = prepare_socket(port);

    for (int i = 0; i < NUM_PORTS; i++) {
        // a negative value is the negative of an OS error code
        if (sockets[i] < 0) {
            fprintf(stderr,
                    "Error preparing sockets; trying to shutdown cleanly...\n");
            for (int j = i - 1; j >= 0; j--) {
                fprintf(stderr, "Closing socket %d\n", sockets[j]);
                close(sockets[j]);
            }
            return -sockets[i]; // flip the sign back to the original error code
        }
    }

    pthread_t threads[NUM_PORTS] = {0};

    for (int i = 0; i < NUM_PORTS; i++) {
        pthread_create(&threads[i], NULL, service_thread, &sockets[i]);
        printf("Thread %ld is listening to port %s on socket %d\n", threads[i],
               argv[i + 1], sockets[i]);
    }

    sem_wait(&shutdown_semaphore);
    printf("\nShutdown requested ...\n");

    for (int i = 0; i < NUM_PORTS; i++) {
        if (sockets[i] > 2) {
            void *unused;
            printf("Closing socket %d\n", sockets[i]);
            pthread_cancel(threads[i]);
            pthread_join(threads[i], &unused);
            close(sockets[i]);
        }
    }
}
