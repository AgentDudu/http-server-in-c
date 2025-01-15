#include "server.h"
#include "router.h"
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

void start_server(int port) {
  WSADATA wsa;
  SOCKET server_socket, client_socket;
  struct sockaddr_in server_addr, client_addr;
  int addr_len;
  int recv_len;
  char buffer[2048];

  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    printf("Failed. Error Code: %d\n", WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
    printf("Could not create socket: %d\n", WSAGetLastError());
    WSACleanup();
    exit(EXIT_FAILURE);
  }

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

  listen(server_socket, 3);
  printf("Waiting for incoming connections on port %d...\n", port);

  while (1) {
    addr_len = sizeof(client_addr);
    printf("Preparing to accept a connection...\n");
    client_socket =
        accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
    if (client_socket == INVALID_SOCKET) {
      printf("Accept failed with error code: %d\n", WSAGetLastError());
      continue;
    }

    recv_len = recv(client_socket, buffer, sizeof(buffer), 0);
    if (recv_len > 0) {
      buffer[recv_len] = '\0';
      handle_request(buffer, client_socket);
    }

    closesocket(client_socket);
  }

  closesocket(server_socket);
  WSACleanup();
}
