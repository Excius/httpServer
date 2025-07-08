#include "server.h"
#include <asm-generic/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

enum httpMethodType {
  HTTP_UNKNOWN = 0,
  HTTP_GET,
  HTTP_POST,
  HTTP_PUT,
  HTTP_PATCH,
  HTTP_DELETE,
};

struct httpMethod {
  const char *const str;
  enum httpMethodType type;
};

const char *const STR_HTTP_GET = "GET";
const char *const STR_HTTP_POST = "POST";
const char *const STR_HTTP_PUT = "PUT";
const char *const STR_HTTP_PATCH = "PATCH";
const char *const STR_HTTP_DELETE = "DELETE";

#define MAX_HEADERS 128

struct httpHeader {
  char *key;
  char *value;
};

struct httpRequest {
  enum httpMethodType method;
  char *path;

  // header
  struct httpHeader *headers;
  size_t headers_len;

  // the request buffer
  char *_buffer;
  size_t _buffer_len;

  // body
  char *body;
};

int server_fd = -1;

void server_start(int port) {
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    printf("Error creating socket\n");
    goto cleanup;
  }

  int yes = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) != 0) {
    printf("Error setting socket options\n");
    goto cleanup;
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    printf("Error binding socket\n");
    goto cleanup;
  }

  if (listen(server_fd, 10) < 0) {
    printf("Error listening on socket\n");
    goto cleanup;
  }

  while (1) {
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {
      printf("error: Error accepting connection: %s \n", strerror(errno));
      continue;
    }

    handle_client(client_fd);
  }

cleanup:
  if (server_fd != -1) {
    close(server_fd);
  }

  return;
}

void handle_client(int client_fd) {
  printf("New client connected: %d\n", client_fd);

  char buffer[8192];
  ssize_t total_bytes = 0;

  total_bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

  if (total_bytes < 0) {
    printf("Error receiving data\n");
    return;
  } else if (total_bytes == 0) {
    printf("Client disconnected\n");
    return;
  }

  buffer[total_bytes] = '\0';

  printf("RAW:\n---BEGIN---\n%.*s\n---END---\n", (int)total_bytes, buffer);

  for (int i = 0; i <= total_bytes; i++) {
    printf("%02X ", (unsigned char)buffer[i]);
  }
  printf("\n");

  char *method = strtok(buffer, " ");
  char *path = strtok(NULL, " ");
  char *version = strtok(NULL, "\r\n");

  char *header = strstr(buffer, "\r\n\r\n");

  printf("Method address: %p\n", header);

  const char *body = "Hello, world!";
  char response[1024];

  if (strcmp(method, "GET") == 0) {
    if (strcmp(path, "/") == 0) {

      snprintf(response, sizeof(response),
               "%s 200 OK\r\n"
               "Content-Length: %zu\r\n"
               "Content-Type: text/plain\r\n"
               "\r\n"
               "%s",
               version, strlen(body), body);

      send(client_fd, response, strlen(response), 0);
    } else if (strcmp(path, "/hello") == 0) {
      const char *body = "Hello, from /hello!";
      snprintf(response, sizeof(response),
               "HTTP/1.1 200 OK\r\n"
               "Content-Length: %zu\r\n"
               "Content-Type: text/plain\r\n"
               "\r\n"
               "%s",
               strlen(body), body);
    } else {

      const char *body = "404 Not Found";
      snprintf(response, sizeof(response),
               "HTTP/1.1 404 Not Found\r\n"
               "Content-Length: %zu\r\n"
               "Content-Type: text/plain\r\n"
               "\r\n"
               "%s",
               strlen(body), body);
    }
  } else {

    const char *body = "405 Method Not Allowed";
    snprintf(response, sizeof(response),
             "HTTP/1.1 405 Method Not Allowed\r\n"
             "Content-Length: %zu\r\n"
             "Content-Type: text/plain\r\n"
             "\r\n"
             "%s",
             strlen(body), body);
  }
}
