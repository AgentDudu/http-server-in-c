#include "router.h"
#include "request.h"
#include "static_handler.h"
#include <stdio.h>
#include <string.h>
#include <winsock2.h>

void log_request(const char *method, const char *url) {
    printf("Method: %s, URL: %s\n", method, url);
}

void log_response(const char *response) {
    printf("Response:\n%s\n", response);
}

void handle_request(const char *request, SOCKET client_socket) {
    char method[16] = {0}, url[256] = {0};
    char response[2048] = {0};
    char body[1024] = {0};
    int body_length;

    parse_request(request, method, url);

    log_request(method, url);

    if (strcmp(url, "/") == 0) {
        snprintf(body, sizeof(body), "Hello World");
        body_length = strlen(body);
        snprintf(response, sizeof(response),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/plain\r\n"
                 "Content-Length: %d\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "%s",
                 body_length, body);
        log_response(response);
        send(client_socket, response, strlen(response), 0);
    } else if (strcmp(url, "/about") == 0) {
        snprintf(body, sizeof(body), "About Page");
        body_length = strlen(body);
        snprintf(response, sizeof(response),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/plain\r\n"
                 "Content-Length: %d\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "%s",
                 body_length, body);
        log_response(response);
        send(client_socket, response, strlen(response), 0);
    } else {
        serve_static_file(url, client_socket);
    }

    shutdown(client_socket, SD_SEND);
    closesocket(client_socket);
}
