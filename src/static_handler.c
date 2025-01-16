#include "static_handler.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <winsock2.h>

void serve_static_file(const char *url, SOCKET client_socket) {
    char path[256];
    char response[2048];
    char body[1024];
    int body_length;
    FILE *file;
    struct stat st;

    snprintf(path, sizeof(path), "static%s", strcmp(url, "/") == 0 ? "/index.html" : url);

    if (stat(path, &st) != 0 || (file = fopen(path, "rb")) == NULL) {
        snprintf(body, sizeof(body), "Not Found");
        body_length = strlen(body);
        snprintf(response, sizeof(response),
                 "HTTP/1.1 404 Not Found\r\n"
                 "Content-Type: text/plain\r\n"
                 "Content-Length: %d\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "%s",
                 body_length, body);
        send(client_socket, response, strlen(response), 0);
        shutdown(client_socket, SD_SEND);
        closesocket(client_socket);
        return;
    }

    body_length = fread(body, 1, sizeof(body), file);
    fclose(file);

    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n"
             "\r\n",
             body_length);

    send(client_socket, response, strlen(response), 0);
    send(client_socket, body, body_length, 0);

    shutdown(client_socket, SD_SEND);
    closesocket(client_socket);
}
