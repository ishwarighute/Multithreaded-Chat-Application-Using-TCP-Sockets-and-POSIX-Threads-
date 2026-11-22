#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int client_socket;

// Thread to receive messages from server
void *receive_messages(void *arg)
{
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while ((bytes_received = recv(client_socket, buffer,
                                  BUFFER_SIZE - 1, 0)) > 0)
    {
        buffer[bytes_received] = '\0';

        printf("\n%s", buffer);
        printf("You: ");
        fflush(stdout);
    }

    printf("\nDisconnected from server.\n");

    exit(0);

    return NULL;
}

// Main client function
int main()
{
    struct sockaddr_in server_address;
    pthread_t receive_thread;

    char message[BUFFER_SIZE];

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    // Configure server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(client_socket,
                (struct sockaddr *)&server_address,
                sizeof(server_address)) < 0)
    {
        perror("Connection failed");
        close(client_socket);
        return 1;
    }

    printf("Connected to Chat Server!\n");
    printf("Type your messages below.\n");
    printf("Type /quit to exit the chat.\n\n");

    // Create thread for receiving messages
    if (pthread_create(&receive_thread, NULL,
                       receive_messages, NULL) != 0)
    {
        perror("Thread creation failed");
        close(client_socket);
        return 1;
    }

    pthread_detach(receive_thread);

    // Send messages to server
    while (1)
    {
        printf("You: ");
        fflush(stdout);

        if (fgets(message, BUFFER_SIZE, stdin) == NULL)
        {
            break;
        }

        // Check exit command
        if (strncmp(message, "/quit", 5) == 0)
        {
            send(client_socket, message, strlen(message), 0);
            break;
        }

        // Send message
        if (send(client_socket, message, strlen(message), 0) < 0)
        {
            perror("Message sending failed");
            break;
        }
    }

    close(client_socket);

    printf("You left the chat.\n");

    return 0;
}