//a basic tcp server
//1.create a socket 
//2.bind the socket to an ip & port
//3.listen for incoming connections from clients
//4.accept them 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

// 1. create a socket and return the file descriptor + bind it
int create_server_socket(int port){
  int server_fd;
  struct sockaddr_in server_addr;  //sin_family, sin_port, sin_addr, ,sin_zero
  server_fd = socket(AF_INET, SOCK_STREAM, 0);//returns a file descriptor, your_process_id + your_file_descriptor => kernels_file_object_pointer
  if(server_fd == -1){
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY; //listen to all interfaces
  server_addr.sin_port = htons(port); //turn to little endian


  if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
    perror("Bind Failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  return server_fd;

}

//2. listen 4 incoming connections

int listen_for_connections(int server_fd){
if(listen(server_fd, 5) <0){
    perror("Listen Failed or Server is busy");
    close(server_fd);
    exit(EXIT_FAILURE);
  }
  printf("Server listening on port..\n");
  return 0;
}

//3.accept a client and return the fd.
int accept_client(int server_fd){
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
  if(client_fd <0){
    perror("Accepting Client Failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }
  char client_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip) );
  printf("Connection established with: %s:%d\n" , client_ip, ntohs(client_addr.sin_port) );
  return client_fd;
}

// handle 1 client

void handle_client(int client_fd){
  struct sockaddr_in client_addr;
  socklen_t addr_len = sizeof(client_addr);
  char buffer[1024];
  char bcopy[1024];
  ssize_t n;
  printf("Handling client connection...\n");
  if (getpeername(client_fd, (struct sockaddr*)&client_addr, &addr_len) == -1) {
        perror("getpeername failed");
        close(client_fd);
        return;
    }
  
  char client_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));

  printf("Client connected from IP: %s, Port: %d\n", client_ip, ntohs(client_addr.sin_port));

  //n = read(client_fd, &buffer, sizeof(buffer));
  //printf("%d", n);
  while ((n = read(client_fd, buffer, sizeof(buffer)))){
   // memset(buffer, 0 ,sizeof(buffer));
   // n = read(client_fd, buffer, sizeof(buffer));
   buffer[n] = '\0';
    //strcpy(bcopy, buffer);
    printf("%s\n ", buffer);
   // n = read(client_fd, &buffer, sizeof(buffer));
    if (write(client_fd, buffer, n) < 0) {
        perror("Write failed");
        close(client_fd);
        return;
    }
  }
  if(n<0){
    perror("Read failed\n");
    printf("reading failed");

  }else if (n==0) {
    printf("Client disconnected\n");
  }
  //echo it back 2 the client -> imp bc this is how it shows up on the clients view
  if (n>0 && write(client_fd, buffer, n) < 0) {
            perror("Write failed");
            close(client_fd);
            return;
        }
  
  close(client_fd);

}


// handle multiple clients using multithreading -> each client gets a new thread 

void* client_thread(void* arg){ //thread function signature
  int client_fd = *((int*)arg); //cast void pointer to int pointer and dereference it
  free(arg);
printf("thread created for cleint \n");
  handle_client(client_fd);
  printf("returned from handle_client");
  return NULL;
}

void handle_multiple_clients(int server_fd){ //the thread created for each client will call the handle_client function via client_thread function.
  while(1){
    int client_fd = accept_client(server_fd);
    pthread_t thread_id;
    int* client_fd_ptr = malloc(sizeof(int));
    *client_fd_ptr = client_fd;

    if(pthread_create(&thread_id,NULL, client_thread, client_fd_ptr) !=0){
      perror("failed to create thread");
      free(client_fd_ptr);
    }
  }
}

void close_server(int server_fd){
  close(server_fd);
  printf("server shut down.\n");

}

int main(){
  int port = 8080;
  int server_fd = create_server_socket(port);
  listen_for_connections(server_fd);
  printf("helo");
  handle_multiple_clients(server_fd);
  close_server(server_fd);
  return 0;
}




