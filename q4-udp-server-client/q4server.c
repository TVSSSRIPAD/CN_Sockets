#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERVERPORT "5000"

#define MAXMSGSIZE 150
// maximum message size

void *getAddress(struct sockaddr *sa)
{
    return &(((struct sockaddr_in *)sa)->sin_addr);
}

int main(void)
{

    struct addrinfo hints, *servinfo, *p;
    int sockfd, rv, bytesSent, bytesRecvd;
    char send_msg[MAXMSGSIZE], recv_msg[MAXMSGSIZE];
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    // set to AF_INET to use IPv4
    hints.ai_family = AF_INET;
    // using UDP Datagrams
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(NULL, SERVERPORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("Server: socket");
            continue;
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("Client: bind");
            continue;
        }
        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "Server: failed to create socket\n");
        return 2;
    }

    addr_len = sizeof their_addr;

    if ((bytesRecvd = recvfrom(sockfd, recv_msg, MAXMSGSIZE - 1, 0, (struct sockaddr *)&their_addr, &addr_len)) == -1)
    {
        perror("recvfrom");
        exit(1);
    }

    printf("Server: Received packet from %s\n", inet_ntop(their_addr.ss_family, getAddress((struct sockaddr *)&their_addr), s, sizeof s));

    recv_msg[bytesRecvd] = '\0';
    printf("Client says \"%s\"\n", recv_msg);

    printf("Enter message to be sent:\n");
    scanf("%[^\n]s", send_msg);

    if ((bytesSent = sendto(sockfd, send_msg, strlen(send_msg), 0, (struct sockaddr *)&their_addr, addr_len)) == -1)
    {
        perror("Server: sendto");
        exit(1);
    }

    freeaddrinfo(servinfo);

    printf("Sent %d bytes  \n", bytesSent);
    close(sockfd);

    return 0;
}