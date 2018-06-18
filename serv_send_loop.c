#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#define N 128

int main(int argc, char *argv[])
{
    int ret; // Success or failure returned value

    in_addr_t srv_addr = INADDR_ANY;
    in_port_t srv_port = htons(0);
    int clients = 10;

    int opt;
    while ((opt = getopt(argc, argv, "a:p:c:")) != -1)
    {
        switch (opt)
        {
        case 'a':
            if (inet_aton(optarg, (struct in_addr *)&srv_addr) != 1)
            {
                fprintf(stderr, "ERROR: IP address conversion failed: %s\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        case 'p':
            srv_port = htons(strtoul(optarg, NULL, 10));
            if (errno == ERANGE)
            {
                fprintf(stderr, "ERROR: Port number conversion failed: %s\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        case 'c':
            clients = strtol(optarg, NULL, 10);
            if (errno == ERANGE)
            {
                fprintf(stderr, "ERROR: The number of clients conversino failed: %s\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        default:
            fprintf(stderr, "Usage: %s -a srv_addr -p srv_port file_to_send\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (argc - optind < 1)
    {
        fprintf(stderr, "Usage: %s -a srv_addr -p srv_port file_to_send\n", argv[0]);
        return EXIT_FAILURE;
    }
    FILE *file = fopen(argv[optind], "rb");
    if (file == NULL)
    {
        fprintf(stderr, "ERROR: Cannot open file %s\n", argv[optind]);
        return EXIT_FAILURE;
    }

    int s = socket(PF_INET, SOCK_STREAM, 0); // TCP socket
    if (s == -1)
    {
        fprintf(stderr, "ERROR: Cannot create a socket: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr;         // IPv4 address & port
    addr.sin_family = AF_INET;       // Mode IPv4
    addr.sin_addr.s_addr = srv_addr; // Server IP address
    addr.sin_port = srv_port;        // Server Port number

    ret = bind(s, (struct sockaddr *)&addr, sizeof(addr)); // Bind to socket
    if (ret == -1)
    {
        fprintf(stderr, "ERROR: Cannot bind port %d: %s\n", ntohs(srv_port), strerror(errno));
        return EXIT_FAILURE;
    }

    ret = listen(s, clients); // Listen to socket
    if (ret == -1)
    {
        fprintf(stderr, "ERROR: Cannot listen to port %d: %s\n", ntohs(srv_port), strerror(errno));
        return EXIT_FAILURE;
    }

    char data[N + 1];
    int n;

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client; // Descriptor number to current client

    while ((client = accept(s, (struct sockaddr *)&client_addr, &client_addr_len)) >= 0)
    {
        char client_addr_str[N];
        if (inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_addr_str, N) == NULL)
        {
            fprintf(stderr, "ERROR: Cannot convert client address to string\n");
            return EXIT_FAILURE;
        }
        fprintf(stdout, "INFO: Accept connection from %s\n", client_addr_str);

        for (int i = 0; i < N; i++)
        {
            while ((n = fread(data, sizeof(char), N, file)) > 0)
            {
                if (send(client, data, n, 0) < n)
                {
                    fprintf(stderr, "ERROR: Cannot send all data\n");
                    return EXIT_FAILURE;
                }
            }
            rewind(file); // Seek cursor of file to the beginning
        }

        ret = shutdown(client, SHUT_WR); // Stop sending data
        if (ret == -1)
        {
            fprintf(stderr, "ERROR: Cannot shutdown the connection to client: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }

        while ((n = recv(client, data, N, 0)) > 0)
        {
            for (int i = 0; i < n; i++)
            {
                putchar(data[i]);
            }
        }

        ret = close(client); // Close the socket
        if (ret == -1)
        {
            fprintf(stderr, "ERROR: Cannot close the socket to client: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }

        rewind(stdin); // Seek cursor of stdin to the beginning

        fprintf(stdout, "INFO: Successfully close connection to %s\n", client_addr_str);
    }
    if (client == -1)
    {
        fprintf(stderr, "ERROR: Cannot accept access from client: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    ret = close(s); // Close the socket
    if (ret == -1)
    {
        fprintf(stderr, "ERROR: Cannot close the socket to server: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    fclose(file);

    return EXIT_SUCCESS;
}