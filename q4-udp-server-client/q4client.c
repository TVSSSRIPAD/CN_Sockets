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

#define PORT "5000"

#define MAXMSGSIZE 150
// maximum size of messages

// get sockaddr IPv4
void *getAddress(struct sockaddr *sa)
{
    return &(((struct sockaddr_in *)sa)->sin_addr);
}

int main(void)
{

    struct addrinfo hints, *servinfo, *p;
    int sockfd, rv, numbytes;
    struct sockaddr_storage their_addr;
    char recv_msg[MAXMSGSIZE], send_msg[MAXMSGSIZE];
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // using IPv4
    hints.ai_socktype = SOCK_DGRAM; // using UDP Datagrams
    hints.ai_flags = AI_PASSIVE;    // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("Client: socket");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "Client: failed to bind socket\n");
        return 2;
    }

    // socket bind() succesfull => servinfo structure is no more required
    freeaddrinfo(servinfo);
    printf("Enter your message :\n");
    scanf("%[^\n]%*c", send_msg);

    // Send query to Server
    if ((numbytes = sendto(sockfd, send_msg, sizeof(send_msg), 0, p->ai_addr, p->ai_addrlen)) == -1)
    {
        perror("send");
        close(sockfd);
    }
    printf("Client: your message is sent\n");
    printf("Client: waiting to receive from server...\n");
    if ((numbytes = recvfrom(sockfd, recv_msg, MAXMSGSIZE, 0, (struct sockaddr *)&their_addr, &addr_len)) == -1)
    {
        perror("recv");
        close(sockfd);
    }
    recv_msg[numbytes] = '\0';
    printf("Client: Message received from Server\n");
    printf("Server says \"%s\" \n", recv_msg);

    close(sockfd);

    return 0;
}