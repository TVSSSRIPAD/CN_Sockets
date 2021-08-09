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

#include <fcntl.h>

#define SERVERPORT "5000"
// the port users will be connecting to

#define MAXMSGSIZE 150
// maximum message size

int MAX_STUDENTS = 10;
// maximum number of students

typedef struct students
{
    char name[MAXMSGSIZE];
    char roll[MAXMSGSIZE];
} students;

int main()
{
    int sockfd; //socket file descriptor
    struct addrinfo hints, *myinfo, *servinfo, *pserv;
    int rv;
    int numbytes;
    char buf[MAXMSGSIZE];
    char ipstr[INET6_ADDRSTRLEN];

    struct sockaddr_storage their_addr;

    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];
    struct timeval tv;

    memset(&hints, 0, sizeof hints);
    // using IPv4 connection
    hints.ai_family = AF_INET;
    // using SOCKDGRAM (UDP)
    hints.ai_socktype = SOCK_DGRAM;

    // get address
    if ((rv = getaddrinfo(NULL, SERVERPORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 2;
    }

    // loop through all addresses and bind to first one we can
    for (pserv = servinfo; (pserv != NULL); pserv = pserv->ai_next)
    {
        if ((sockfd = socket(pserv->ai_family, pserv->ai_socktype, pserv->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }
        if (bind(sockfd, pserv->ai_addr, pserv->ai_addrlen) == -1)
        {
            perror("bind");
            close(sockfd);
            continue;
        }
    }

    int len;
    char line[150];

    // opening file with role numbers and passwords
    FILE *fp = fopen("roll_name.txt", "r");

    // reading names, roll numbers and storing them in students structure

    int i = 0, j = 0, k = 0;
    students stu[MAX_STUDENTS];
    while (len = fgets(line, 150, fp) > 0)
    {
        // printf("%s %dn", line, sizeof(line));
        j = 0;
        while (line[j] != ' ')
        {
            stu[i].name[j] = line[j];
            // printf("\n %c \n", line[j]);
            j++;
        }
        stu[i].name[j] = '\0';
        j++;
        k = 0;
        while (line[j] != '\n')
        {
            stu[i].roll[k] = line[j];
            // printf("  %c ", line[j]);
            j++;
            k++;
        }
        stu[i].roll[k] = '\0';
        // printf("\n");
        i++;
        // printf("i is %d\n", i);
    }

    MAX_STUDENTS = i;

    while (1)
    {
        addr_len = sizeof their_addr;
        int k;
        for (k = 0; k < 150; k++)
        {
            buf[k] = '\0';
        }
        if ((numbytes = recvfrom(sockfd, buf, MAXMSGSIZE - 1, 0, (struct sockaddr *)&their_addr, &addr_len)) == -1)
        {
            perror("recvfrom");
            exit(1);
        }
        // printf("server: packet is %d bytes long\n", numbytes);
        buf[numbytes] = '\0';

        if (buf[0] == '1')
        {
            // Request for name received
            char roll_rec[MAXMSGSIZE];
            int j, ll;
            for (ll = 2; buf[ll]; ll++)
            {
                roll_rec[ll - 2] = buf[ll];
            }
            roll_rec[ll - 2] = '\0';
            int f = 0, k;
            printf("server: received request for name of \"%s\"\n", roll_rec);
            for (j = 0; j < MAX_STUDENTS; j++)
            {

                if (strcmp(roll_rec, stu[j].roll) == 0)
                {
                    f = 1;
                    break;
                }
            }
            char mname[150];
            char *x = stu[j].name;
            if (f)
            {
                if ((numbytes = sendto(sockfd, x, 150, 0, (struct sockaddr *)&their_addr, addr_len)) == -1)
                {
                    perror("sendto");
                    exit(1);
                }
            }
            else
            {
                if ((numbytes = sendto(sockfd, "No student found", 17, 0, (struct sockaddr *)&their_addr, addr_len)) == -1)
                {
                    perror("sendto");
                    exit(1);
                }
            }
            // printf("server: sent packet is %d bytes long\n", numbytes);
        }
        else if (buf[0] == '2')
        {
            // Request for roll number received
            char name_rec[MAXMSGSIZE];
            int j, ll;
            for (ll = 2; buf[ll]; ll++)
            {
                name_rec[ll - 2] = buf[ll];
                // printf("%c ", buf[ll]);
            }
            name_rec[ll - 2] = '\0';
            int f = 0, k;
            printf("server: received request for roll number of \"%s\"\n", name_rec);
            for (j = 0; j < MAX_STUDENTS; j++)
            {

                if (strcmp(name_rec, stu[j].name) == 0)
                {
                    f = 1;
                    break;
                }
            }

            char *x = stu[j].roll;
            if (f)
            {
                if ((numbytes = sendto(sockfd, x, 150, 0, (struct sockaddr *)&their_addr, addr_len)) == -1)
                {
                    perror("sendto");
                    exit(1);
                }
            }
            else
            {
                if ((numbytes = sendto(sockfd, "No student found", 17, 0, (struct sockaddr *)&their_addr, addr_len)) == -1)
                {
                    perror("sendto");
                    exit(1);
                }
            }
        }
    }

    freeaddrinfo(servinfo);

    close(sockfd);

    return 0;
}