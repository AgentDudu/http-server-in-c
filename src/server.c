#include "server.h"
#include "router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

DWORD WINAPI handle_client(LPVOID client_socket_ptr) {
    SOCKET client_socket = *(SOCKET *)client_socket_ptr;
    char buffer[2048] = {0};
    int recv_len;

    recv_len = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (recv_len > 0) {
        buffer[recv_len] = '\0';
        handle_request(buffer, client_socket);
    }

    shutdown(client_socket, SD_SEND);
    closesocket(client_socket);
    free(client_socket_ptr);

    return 0;
}

void start_server(int port) {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_len;

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

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed with error code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    listen(server_socket, 5);
    printf("Server is listening on port %d...\n", port);

    addr_len = sizeof(struct sockaddr_in);
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed with error code: %d\n", WSAGetLastError());
            continue;
        }

        printf("New client connected.\n");

        SOCKET *client_socket_ptr = malloc(sizeof(SOCKET));
        if (client_socket_ptr == NULL) {
            printf("Memory allocation failed.\n");
            closesocket(client_socket);
            continue;
        }
        *client_socket_ptr = client_socket;

        HANDLE thread = CreateThread(NULL, 0, handle_client, client_socket_ptr, 0, NULL);
        if (thread == NULL) {
            printf("Failed to create thread: %d\n", GetLastError());
            free(client_socket_ptr);
            closesocket(client_socket);
        } else {
            CloseHandle(thread);
        }
    }

    closesocket(server_socket);
    WSACleanup();
}
