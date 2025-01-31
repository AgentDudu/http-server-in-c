#include "server.h"
#include "logger.h"
#include "router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

char client_ip[32]; 
double request_duration;

DWORD WINAPI handle_client(LPVOID client_socket_ptr) {
    SOCKET client_socket = *(SOCKET *)client_socket_ptr;
    struct sockaddr_in client_addr;
    int addr_len = sizeof(client_addr);

    if (getpeername(client_socket, (struct sockaddr *)&client_addr, &addr_len) == 0) {
        strcpy(client_ip, inet_ntoa(client_addr.sin_addr)); // Store IP globally

        char buffer[2048] = {0};
        int recv_len;
        clock_t start_time = clock(); // Start timing request

        recv_len = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (recv_len > 0) {
            buffer[recv_len] = '\0';
            handle_request(buffer, client_socket);
        } else if (recv_len == SOCKET_ERROR) {
            log_error("Error receiving data from client.");
            const char *error_response = "HTTP/1.1 500 Internal Server Error\r\n"
                                         "Content-Type: text/plain\r\n"
                                         "Content-Length: 21\r\n"
                                         "Connection: close\r\n"
                                         "\r\n"
                                         "Internal Server Error";
            send(client_socket, error_response, strlen(error_response), 0);
        }

        clock_t end_time = clock();
        request_duration = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000; // Store duration globally

        shutdown(client_socket, SD_SEND);
        closesocket(client_socket);
        free(client_socket_ptr);
    }
    return 0;
}

void start_server(int port) {
  WSADATA wsa;
  SOCKET server_socket, client_socket;
  struct sockaddr_in server_addr, client_addr;
  int addr_len;

  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    log_error("WSAStartup failed.");
    exit(EXIT_FAILURE);
  }

  if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
    log_error("Failed to create server socket.");
    WSACleanup();
    exit(EXIT_FAILURE);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  if (bind(server_socket, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) == SOCKET_ERROR) {
    log_error("Bind failed.");
    closesocket(server_socket);
    WSACleanup();
    exit(EXIT_FAILURE);
  }

  listen(server_socket, 5);
  printf("Server is listening on port %d...\n", port);

  addr_len = sizeof(struct sockaddr_in);
  while (1) {
    client_socket =
        accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
    if (client_socket == INVALID_SOCKET) {
      log_error("Accept failed.");
      continue;
    }

    SOCKET *client_socket_ptr = malloc(sizeof(SOCKET));
    if (client_socket_ptr == NULL) {
      log_error("Memory allocation failed.");
      closesocket(client_socket);
      continue;
    }
    *client_socket_ptr = client_socket;

    HANDLE thread =
        CreateThread(NULL, 0, handle_client, client_socket_ptr, 0, NULL);
    if (thread == NULL) {
      log_error("Failed to create thread.");
      free(client_socket_ptr);
      closesocket(client_socket);
    } else {
      CloseHandle(thread);
    }
  }

  closesocket(server_socket);
  WSACleanup();
}
