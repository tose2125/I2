#include "net.h"

int parse_optarg_client(int argc, char *argv[], in_addr_t *dst_addr, in_port_t *dst_port)
{
    int ret = 0; // Success or failure to return value

    int opt;
    while ((opt = getopt(argc, argv, "a:p:")) != -1)
    {
        switch (opt)
        {
        case 'a':
            if (inet_aton(optarg, (struct in_addr *)dst_addr) != 1)
            {
                fprintf(stderr, "ERROR: Failed to convert IP address: %s\n", optarg);
                ret = 1;
            }
            break;
        case 'p':
            errno = 0;
            *dst_port = htons(strtoul(optarg, NULL, 10));
            if (errno == ERANGE)
            {
                fprintf(stderr, "ERROR: Failed to convert port number: %s\n", optarg);
                ret = 1;
            }
            break;
        default:
            fprintf(stderr, "Usage: %s -a dst_addr -p dst_port\n", argv[0]);
            ret = 1;
        }
    }

    return ret;
}

int connect_tcp_client(in_addr_t *dst_addr, in_port_t *dst_port)
{
    int ret; // Success or failure returned value

    int sock = socket(PF_INET, SOCK_STREAM, 0); // TCP socket
    if (sock == -1)
    {
        fprintf(stderr, "ERROR: Failed to create a socket: %s\n", strerror(errno));
        return -1;
    }

    if (dst_addr == NULL || dst_port == NULL)
    {
        fprintf(stderr, "ERROR: Specify destination\n");
        return -1;
    }

    struct sockaddr_in addr;          // IPv4 address & port
    addr.sin_family = AF_INET;        // Mode IPv4
    addr.sin_addr.s_addr = *dst_addr; // Destination IP address
    addr.sin_port = *dst_port;        // Destination Port number

    ret = connect(sock, (struct sockaddr *)&addr, sizeof(addr)); // Connect to destination
    if (ret == -1)
    {
        fprintf(stderr, "ERROR: Failed to connect to remote: %s\n", strerror(errno));
        return -1;
    }

    return sock;
}

int connect_udp_client(in_addr_t *dst_addr, in_port_t *dst_port)
{
    int ret; // Success or failure returned value

    int sock = socket(PF_INET, SOCK_DGRAM, 0); // UDP socket
    if (sock == -1)
    {
        fprintf(stderr, "ERROR: Failed to create a socket: %s\n", strerror(errno));
        return -1;
    }

    if (dst_addr == NULL || dst_port == NULL)
    {
        return sock;
    }

    struct sockaddr_in addr;          // IPv4 address & port
    addr.sin_family = AF_INET;        // Mode IPv4
    addr.sin_addr.s_addr = *dst_addr; // Destination IP address
    addr.sin_port = *dst_port;        // Destination Port number

    char data = UDP_EOD;
    ret = sendto(sock, &data, 1, 0, (struct sockaddr *)&addr, sizeof(addr)); // Connect to destination
    if (ret < 1)
    {
        fprintf(stderr, "ERROR: Failed to connect to remote: %s\n", strerror(errno));
        return -1;
    }

    return sock;
}
