#include <stdio.h>  // stderr
#include <stdlib.h> // exit
#include <unistd.h> // close
#include "net.h"

#define N 1024

int client_send_recv(int sock, in_addr_t dst_addr, in_port_t dst_port)
{
    char data[N + 1];
    int n;

    struct sockaddr_in addr;         // IPv4 address & port
    addr.sin_family = AF_INET;       // Mode IPv4
    addr.sin_addr.s_addr = dst_addr; // Destination IP address
    addr.sin_port = dst_port;        // Destination Port number

    while (1)
    {
        if ((n = fread(data, sizeof(char), N, stdin)) == 0)
        {
            for (int i = 0; i < N; i++)
            {
                data[i] = '\n';
            }
            if (sendto(sock, data, N, 0, (struct sockaddr *)&addr, sizeof(addr)) < n) // Send EOD
            {
                fprintf(stderr, "ERROR: Failed to send all data\n");
                return EXIT_FAILURE;
            }
            break;
        }
        if (sendto(sock, data, n, 0, (struct sockaddr *)&addr, sizeof(addr)) < n) // Send Data from stdin
        {
            fprintf(stderr, "ERROR: Failed to send all data\n");
            return EXIT_FAILURE;
        }
    }

    struct sockaddr *src_addr = NULL; // IPv4 address & port
    socklen_t *src_addr_len = NULL;

    while (1)
    {
        if ((n = recvfrom(sock, data, N, 0, src_addr, src_addr_len)) == 0)
        {
            fprintf(stderr, "INFO: Receive 0 bytes\n");
            break;
        }
        if (n == N)
        {
            int eod = 1;
            for (int i = 0; i < n; i++)
            {
                if (data[i] != UDP_EOD)
                {
                    eod = 0;
                    break;
                }
            }
            if (eod)
            {
                break;
            }
        }
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

    in_addr_t dst_addr = inet_addr("127.0.0.1");
    in_port_t dst_port = htons(50000);
    ret = parse_optarg_client(argc, argv, &dst_addr, &dst_port);
    if (ret != 0)
    {
        fprintf(stderr, "ERROR: Failed to parse option arguments\n");
        exit(EXIT_FAILURE);
    }

    int sock = connect_udp_client(NULL, NULL); // UDP socket
    if (sock == -1)
    {
        fprintf(stderr, "ERROR: Failed to establish connection\n");
        exit(EXIT_FAILURE);
    }

    client_send_recv(sock, dst_addr, dst_port); // Receive data from remote and print to stdout after send data from stdin to remote

    ret = close(sock); // Close the socket
    if (ret == -1)
    {
        fprintf(stderr, "ERROR: Failed to close the socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
