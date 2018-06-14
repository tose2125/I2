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

    in_addr_t dst_addr = inet_addr("127.0.0.1");
    in_port_t dst_port = htons(50000);

    int opt;
    while ((opt = getopt(argc, argv, "a:p:")) != -1)
    {
        switch (opt)
        {
        case 'a':
            if (inet_aton(optarg, (struct in_addr *)&dst_addr) != 1)
            {
                fprintf(stderr, "ERROR: IP address conversion failed: %s\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        case 'p':
            dst_port = htons(strtoul(optarg, NULL, 10));
            if (errno == ERANGE)
            {
                fprintf(stderr, "ERROR: Port number conversion failed: %s\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        default:
            fprintf(stderr, "Usage: %s -a dst_addr -p dst_port\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    int s = socket(PF_INET, SOCK_STREAM, 0); // TCP socket
    if (s == -1)
    {
        fprintf(stderr, "ERROR: Cannot create a socket: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr;         // IPv4 address & port
    addr.sin_family = AF_INET;       // Mode IPv4
    addr.sin_addr.s_addr = dst_addr; // Destination IP address
    addr.sin_port = dst_port;        // Destination Port number

    ret = connect(s, (struct sockaddr *)&addr, sizeof(addr)); // Connect to destination
    if (ret == -1)
    {
        fprintf(stderr, "ERROR: Cannot connect to remote: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    char data[N + 1];
    int n;
    while (1)
    {
        if ((n = recv(s, data, N, 0)) == 0)
        {
            break;
        }
        for (int i = 0; i < n; i++)
        {
            putchar(data[i]);
        }
    }

    /*ret = shutdown(s, SHUT_WR); // Stop sending data
    if (ret == -1)
    {
        fprintf(stderr, "ERROR: Cannot shutdown the connection: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }*/

    ret = close(s); // Close the socket
    if (ret == -1)
    {
        fprintf(stderr, "ERROR: Cannot close the socket: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
