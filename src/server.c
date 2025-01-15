#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

void start_server(int port) {
  WSADATA wsa;
  SOCKET server_socket, client_socket;
  struct sockaddr_in server_addr, client_addr;
  int addr_len, recv_len;
  char buffer[1024];

  printf("Initializing Winsock...\n");
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    printf("Failed. Error Code: %d\n", WSAGetLastError());
    exit(EXIT_FAILURE);
  }
  printf("Initialized.\n");

  if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
    printf("Could not create socket: %d\n", WSAGetLastError());
    WSACleanup();
    exit(EXIT_FAILURE);
  }
  printf("Socket created.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  if (bind(server_socket, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) == SOCKET_ERROR) {
    printf("Bind failed with error code: %d\n", WSAGetLastError());
    closesocket(server_socket);
    WSACleanup();
    exit(EXIT_FAILURE);
  }
  printf("Bind done.\n");

  listen(server_socket, 3);
  printf("Waiting for incoming connections on port %d...\n", port);

  addr_len = sizeof(struct sockaddr_in);
  while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr,
                                 &addr_len)) != INVALID_SOCKET) {
    printf("Connection accepted.\n");

    recv_len = recv(client_socket, buffer, sizeof(buffer), 0);
    if (recv_len > 0) {
      buffer[recv_len] = '\0';
      printf("Received: %s\n", buffer);

      const char *response = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/plain\r\n"
                             "Content-Length: 12\r\n"
                             "\r\n"
                             "Hello World";
      send(client_socket, response, strlen(response), 0);
    }

    closesocket(client_socket);
    printf("Client disconnected.\n");
  }

  if (client_socket == INVALID_SOCKET) {
    printf("Accept failed with error code: %d\n", WSAGetLastError());
  }

  closesocket(server_socket);
  WSACleanup();
}
