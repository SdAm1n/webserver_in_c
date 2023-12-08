// Author: Md Sadikul Amin Sadman
// Github: SdAm1n
// Basic Webserver in C programming language

#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// signal handler for CTRL-C
void signal_handler(int sig)
{
    printf("\nCTRL-C pressed, Exiting Server...\n");
    exit(sig);
}

int main()
{
    char buffer[BUFFER_SIZE];

    // hardcoding the response
    char resp[] = "HTTP/1.0 200 OK\r\n"
                  "Server: webserver-c\r\n"
                  "Content-type: text/html\r\n\r\n"
                  "<html>Welcome to this world!</html>\r\n";

    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Socket Creation Failed");
        return 1;
    }

    printf("Socket Created...\n");

    // Create an address to bind the socket to that address
    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);

    // set the address
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // get client address
    struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);

    // Bind Socket to the address
    if (bind(sockfd, (struct sockaddr *)&host_addr, host_addrlen) != 0)
    {
        perror("Socket Binding Failed");
        return 1;
    }

    printf("Socket binded...\n");

    // Listen for incoming connections
    if (listen(sockfd, SOMAXCONN) != 0)
    {
        perror("Listen Failed");
        return 1;
    }

    printf("Server listening...\n");

    // register signal handler for CTRL-C
    signal(SIGINT, signal_handler);

    // For Accepting incoming connections
    // loop to continue accepting connections
    while (1)
    {
        // accept incoming connection
        int newsockfd = accept(sockfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);
        if (newsockfd < 0)
        {
            perror("Server Accept Failed");
            continue;
        }
        printf("Server accepted the client...\n");

        // Get client address
        int sockn = getsockname(newsockfd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addrlen);
        if (sockn < 0)
        {
            perror("Server getsockname Failed");
            continue;
        }

        // read from the socket
        int valread = read(newsockfd, buffer, BUFFER_SIZE);
        if (valread < 0)
        {
            perror("Server read Failed");
            continue;
        }

        char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];

        // print the full request
        printf("Printing Client Request...\n");
        printf("%s\n", buffer);

        // parse the request
        sscanf(buffer, "%s %s %s", method, uri, version);

        printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);

        // write to the socket
        int valwrite = write(newsockfd, resp, strlen(resp));
        if (valwrite < 0)
        {
            perror("Server write Failed");
            continue;
        }

        close(newsockfd);
    }

    return 0;
}