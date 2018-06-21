#include <stdio.h>  // stderr
#include <stdlib.h> // exit
#include <unistd.h> // close
#include "net.h"

#define N 128

int client_send_recv(int sock)
{
    char data[N + 1]; // Buffer

    int n, i;
    while ((n = fread(data, sizeof(char), N, stdin)) > 0)
    {
        if (send(sock, data, n, 0) < n)
        {
            fprintf(stderr, "ERROR: Failed to send all data\n");
            return EXIT_FAILURE;
        }
    }

    if (shutdown(sock, SHUT_WR) == -1)// Stop sending data
    {
        fprintf(stderr, "ERROR: Failed to shutdown the connection: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    while ((n = recv(sock, data, N, 0)) > 0)
    {
        for (i = 0; i < n; i++)
        {
            putchar(data[i]);
        }
    }

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    int ret; // Success or failure returned value

    in_addr_t dst_addr = inet_addr("127.0.0.1");
    in_port_t dst_port = htons(50000);
    ret = parse_optarg_client(argc, argv, &dst_addr, &dst_port);
    if (ret != 0)
    {
        fprintf(stderr, "ERROR: Failed to parse option arguments\n");
        exit(EXIT_FAILURE);
    }

    int sock = connect_tcp_client(&dst_addr, &dst_port); // TCP socket
    if (sock == -1)
    {
        fprintf(stderr, "ERROR: Failed to establish connection\n");
        exit(EXIT_FAILURE);
    }

    client_send_recv(sock); // Receive data from remote and print to stdout after send data from stdin to remote

    ret = close(sock); // Close the socket
    if (ret == -1)
    {
        fprintf(stderr, "ERROR: Failed to close the socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
