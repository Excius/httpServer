#include "server.h"
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void server_start(int port) {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0) {
    printf("Error creating socket\n");
    return;
  }

  int yes = 1;
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) != 0) {
    printf("Error setting socket options\n");
    return;
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    printf("Error binding socket\n");
    return;
  }

  if (listen(socket_fd, 10) < 0) {
    printf("Error listening on socket\n");
    return;
  }

  while (1) {
    int client_fd = accept(socket_fd, NULL, NULL);
    if (client_fd < 0) {
      printf("Error accepting connection\n");
      continue;
    }

    handle_client(client_fd);
    close(client_fd);
  }
}

void handle_client(int client_fd) {
  printf("New client connected: %d\n", client_fd);

  char buffer[4096];

  ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

  if (bytes_received < 0) {
    printf("Error receiving data\n");
    return;
  } else if (bytes_received == 0) {
    printf("Client disconnected\n");
    return;
  }

  printf("Received:\n%.*s\n", (int)bytes_received, buffer);

  const char *response = "HTTP/1.1 200 OK\r\n"
                         "Content-Length: 22\r\n"
                         "Content-Type: text/html\r\n"
                         "\r\n"
                         "<H1>Hello, world!</H1>\r\n";

  send(client_fd, response, strlen(response), 0);
}
