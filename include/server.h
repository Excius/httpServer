#ifndef SERVER_H
#define SERVER_H

// Declare the main entry point for the server
void server_start(int port);

// Declare your client handler
void handle_client(int client_fd);

#endif // SERVER_H
