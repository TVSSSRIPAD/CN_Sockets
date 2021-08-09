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
#include <stdbool.h>

#define LOCALPORT "5000"
#define MAXMSGSIZE 150

int main()
{
    int sockfd;
    struct addrinfo hints, *myinfo, *servinfo, *pserv, *plocal;
    int rv;
    int numbytes;
    char buf[MAXMSGSIZE], send_msg[MAXMSGSIZE];
    // get all of the server addresses
    memset(&hints, 0, sizeof hints);

    // using IPv4
    hints.ai_family = AF_INET;
    // using UDP Datagrams
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(NULL, LOCALPORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 2;
    }

    // loop through all the server addresses
    for (pserv = servinfo; (pserv != NULL); pserv = pserv->ai_next)
    {

        if ((sockfd = socket(pserv->ai_family, pserv->ai_socktype, pserv->ai_protocol)) == -1)
        {
            perror("socket");
            continue;
        }

        break;
    }
    if (pserv == NULL)
    {
        fprintf(stderr, "client: failed to create socket\n");
        return 2;
    }
    while (1)
    {
        printf("Enter 1 followed by Roll Number to get name\n or  2 followed by Name to get Roll number\n Any other character to exit\n");
        scanf("%[^\n]%*c", send_msg);
        if (send_msg[0] != '2' && send_msg[0] != '1')
            break;

        // Send query to Server
        if ((numbytes = sendto(sockfd, send_msg, sizeof(send_msg), 0, pserv->ai_addr, pserv->ai_addrlen)) == -1)
        {
            perror("send");
            close(sockfd);
        }

        // printf("client: sent %d bytes\n", numbytes);

        struct sockaddr_storage their_addr;

        socklen_t addr_len;

        // Obtain response from Server ad print it
        if ((numbytes = recvfrom(sockfd, buf, MAXMSGSIZE, 0, (struct sockaddr *)&their_addr, &addr_len)) == -1)
        {
            perror("recv");
            close(sockfd);
        }
        buf[numbytes] = '\0';
        // printf("client: received %d bytes\n", numbytes);
        if (send_msg[0] == '1')
        {
            printf("Name of student with given roll number is: %s \n", buf);
        }
        else if (send_msg[0] == '2')
        {
            printf("Roll number of student with given name is: %s \n", buf);
        }
    }

    close(sockfd);
    freeaddrinfo(servinfo);

    return 0;
}