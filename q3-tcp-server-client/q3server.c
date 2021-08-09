#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT "5000"
// the port users will be connecting to

#define BACKLOG 1
// the no of connections queue will hold

#define MAXMSGSIZE 150
// maximum size of messages

// get IPv4  sockaddr  ;
void *getAddress(struct sockaddr *sa)
{
    return &(((struct sockaddr_in *)sa)->sin_addr);
}

int main(void)
{

    // listen on sock_fd and get new connection on sock_conn_fd
    int sockfd, sock_conn_fd;

    struct addrinfo hints, *servinfo, *loopvar;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;

    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    char recv_msg[MAXMSGSIZE];
    int rv, bytesRecvd;

    memset(&hints, 0, sizeof hints);
    //using IPv4
    hints.ai_family = AF_INET;
    // using TCP Sockets
    hints.ai_socktype = SOCK_STREAM;
    // use my IP address
    hints.ai_flags = AI_PASSIVE;

    //Get address information of Port
    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (loopvar = servinfo; loopvar != NULL; loopvar = loopvar->ai_next)
    {
        // Create Socket
        if ((sockfd = socket(loopvar->ai_family, loopvar->ai_socktype, loopvar->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }
        // Binding socket
        if (bind(sockfd, loopvar->ai_addr, loopvar->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    // servinfo structure is no more required as socket bind() is completed
    freeaddrinfo(servinfo);

    if (loopvar == NULL)
    {
        fprintf(stderr, "Server: failed to bind\n");
        exit(1);
    }

    // Listen for incoming requests
    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    printf("Server: waiting for connection...\n");

    sin_size = sizeof their_addr;

    // Accept connections
    sock_conn_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (sock_conn_fd == -1)
    {
        perror("accept");
    }

    inet_ntop(their_addr.ss_family, getAddress((struct sockaddr *)&their_addr), s, sizeof s);

    //Printing address of client
    printf("Server: got connection from %s\n", s);

    close(sockfd); // listener is no more required

    // Receive message from client
    if ((bytesRecvd = recv(sock_conn_fd, recv_msg, MAXMSGSIZE - 1, 0)) == -1)
    {
        perror("recv");
        exit(1);
    }
    recv_msg[bytesRecvd] = '\0';

    printf("Server: Message received from Client\n");
    printf("Client says '%s' \n", recv_msg);

    close(sock_conn_fd); // closing the connection

    return 0;
}