#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

int clients[MAX_CLIENTS];
int client_count = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Broadcast message to all connected clients
void broadcast_message(char *message, int sender)
{
    pthread_mutex_lock(&lock);

    for (int i = 0; i < client_count; i++)
    {
        if (clients[i] != sender)
        {
            send(clients[i], message, strlen(message), 0);
        }
    }

    pthread_mutex_unlock(&lock);
}

// Remove disconnected client
void remove_client(int socket)
{
    pthread_mutex_lock(&lock);

    for (int i = 0; i < client_count; i++)
    {
        if (clients[i] == socket)
        {
            for (int j = i; j < client_count - 1; j++)
            {
                clients[j] = clients[j + 1];
            }

            client_count--;
            break;
        }
    }

    pthread_mutex_unlock(&lock);
}

// Handle individual client
void *handle_client(void *arg)
{
    int client_socket = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    int bytes_received;

    printf("Client connected: %d\n", client_socket);

    while ((bytes_received = recv(client_socket, buffer,
                                  BUFFER_SIZE - 1, 0)) > 0)
    {
        buffer[bytes_received] = '\0';

        if (strncmp(buffer, "/quit", 5) == 0)
        {
            break;
        }

        printf("Received: %s", buffer);

        broadcast_message(buffer, client_socket);
    }

    remove_client(client_socket);

    close(client_socket);

    printf("Client disconnected: %d\n", client_socket);

    return NULL;
}

// Main server function
int main()
{
    int server_socket;
    struct sockaddr_in server_address;

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    // Configure server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(server_socket, (struct sockaddr *)&server_address,
             sizeof(server_address)) < 0)
    {
        perror("Bind failed");
        close(server_socket);
        return 1;
    }

    // Listen for clients
    if (listen(server_socket, MAX_CLIENTS) < 0)
    {
        perror("Listen failed");
        close(server_socket);
        return 1;
    }

    printf("Chat Server Started!\n");
    printf("Waiting for clients on port %d...\n", PORT);

    // Accept multiple clients
    while (1)
    {
        int client_socket = accept(server_socket, NULL, NULL);

        if (client_socket < 0)
        {
            perror("Accept failed");
            continue;
        }

        pthread_mutex_lock(&lock);

        if (client_count >= MAX_CLIENTS)
        {
            pthread_mutex_unlock(&lock);
            send(client_socket, "Server is full\n", 15, 0);
            close(client_socket);
            continue;
        }

        clients[client_count++] = client_socket;

        pthread_mutex_unlock(&lock);

        // Create separate thread for each client
        pthread_t thread;

        int *new_socket = malloc(sizeof(int));
        *new_socket = client_socket;

        if (pthread_create(&thread, NULL, handle_client, new_socket) != 0)
        {
            perror("Thread creation failed");
            free(new_socket);
            remove_client(client_socket);
            close(client_socket);
            continue;
        }

        pthread_detach(thread);
    }

    close(server_socket);

    return 0;
}