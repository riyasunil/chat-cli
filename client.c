#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int create_client_socket(const char* server_ip, int server_port) {
    int client_fd;
    struct sockaddr_in server_addr;

    // Create socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    
    // Convert server IP address from string to binary
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid server IP address");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    return client_fd;
}

void connect_to_server(int client_fd) {
    struct sockaddr_in server_addr;

    // Connect to the server
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server.\n");
}

void send_message(int client_fd, const char* message) {
    if (write(client_fd, message, strlen(message)) < 0) {
        perror("Send message failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
  //  printf("Message sent: %s\n", message);
}

void receive_message(int client_fd) {
    char buffer[1024];
    ssize_t n;

    // Receive a message from the server
    n = read(client_fd, buffer, sizeof(buffer) - 1);
    if (n < 0) {
        perror("Receive message failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    buffer[n] = '\0'; // Null-terminate the string
    printf("Received from server: %s\n", buffer);
}
void interactive_client(int client_fd) {
    char message[1024];

    while (1) {
        printf("Enter message: ");
        fgets(message, sizeof(message), stdin);

        // Remove the newline character at the end of the input string
        message[strcspn(message, "\n")] = '\0';
//message[strlen(message)-1] = "\0";
 //   printf("%s\n", message);
 // printf("%c", message[strlen(message)-1]);
        if (strcmp(message, "exit") == 0) {
            printf("Exiting...\n");
            break;
        }

        send_message(client_fd, message);
        receive_message(client_fd);
    }
}
void close_connection(int client_fd) {
    close(client_fd);
    printf("Connection closed.\n");
}
int main() {
    const char* server_ip = "127.0.0.1"; // Local server IP
    int server_port = 8080;              // Port to connect to

    // Create client socket
    int client_fd = create_client_socket(server_ip, server_port);
    
    // Connect to the server
    connect_to_server(client_fd);

    // Start interactive communication with the server
    interactive_client(client_fd);

    // Close the connection when done
    close_connection(client_fd);

    return 0;
}

