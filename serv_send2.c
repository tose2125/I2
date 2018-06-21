#include <stdio.h>  // stderr
#include <stdlib.h> // exit
#include <unistd.h> // close
#include "net.h"

#define N 128

int serv_send2(int sock)
{
    int ret; // Success or failure returned value

    const char rec_command[128] = "rec -t raw -b 16 -c 1 -e s -r 44100 - ";
    FILE *rec = popen(rec_command, "r");
    if (rec == NULL)
    {
        fprintf(stderr, "ERROR: Failed to open process: %s\n", rec_command);
        return EXIT_FAILURE;
    }

    char data[N + 1];
    int n;

    while ((n = fread(data, sizeof(char), N, rec)) > 0)
    {
        if (send(sock, data, n, 0) < n)
        {
            fprintf(stderr, "ERROR: Failed to send all data\n");
            return EXIT_FAILURE;
        }
    }

    ret = pclose(rec);
    if (ret == -1)
    {
        fprintf(stderr, "ERROR: Failed to close the record process: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    ret = shutdown(sock, SHUT_WR); // Stop sending data
    if (ret == -1)
    {
        fprintf(stderr, "ERROR: Failed to shutdown the connection to client: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    while ((n = recv(sock, data, N, 0)) > 0)
    {
        for (int i = 0; i < n; i++)
        {
            putchar(data[i]);
        }
    }

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    int ret; // Success or failure returned value

    in_addr_t srv_addr = INADDR_ANY;
    in_port_t srv_port = htons(0);
    int lsn_limit = 10;
    ret = parse_optarg_server(argc, argv, &srv_addr, &srv_port, &lsn_limit);
    if (ret != 0)
    {
        fprintf(stderr, "ERROR: Failed to parse option arguments\n");
        exit(EXIT_FAILURE);
    }

    int sock = listen_tcp_server(&srv_addr, &srv_port, lsn_limit);
    if (sock == -1)
    {
        fprintf(stderr, "ERROR: Failed to establish server\n");
        exit(EXIT_FAILURE);
    }

    ret = handle_tcp_server(sock, serv_send2);
    if (ret == -1)
    {
        fprintf(stderr, "ERROR: Failed to correctly serve: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    ret = close(sock); // Close the socket
    if (ret == -1)
    {
        fprintf(stderr, "ERROR: Failed to close the socket to server: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
