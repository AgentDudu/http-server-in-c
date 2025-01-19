#include "router.h"
#include "request.h"
#include "static_handler.h"
#include "logger.h"
#include <stdio.h>
#include <string.h>
#include <winsock2.h>

void handle_request(const char *request, SOCKET client_socket) {
    char method[16] = {0}, url[256] = {0}, protocol[16] = {0};
    char response[2048] = {0};
    char body[1024] = {0};
    int body_length;

    if (sscanf(request, "%15s %255s %15s", method, url, protocol) != 3) {
        log_error("Malformed HTTP request.");
        snprintf(response, sizeof(response),
                 "HTTP/1.1 400 Bad Request\r\n"
                 "Content-Type: text/plain\r\n"
                 "Content-Length: 11\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "Bad Request");
        send(client_socket, response, strlen(response), 0);
        return;
    }

    if (strcmp(protocol, "HTTP/1.1") != 0) {
        log_error("Unsupported protocol version.");
        snprintf(response, sizeof(response),
                 "HTTP/1.1 505 HTTP Version Not Supported\r\n"
                 "Content-Type: text/plain\r\n"
                 "Content-Length: 26\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "HTTP Version Not Supported");
        send(client_socket, response, strlen(response), 0);
        return;
    }

    if (strcmp(method, "GET") != 0) {
        log_error("Unsupported HTTP method.");
        snprintf(response, sizeof(response),
                 "HTTP/1.1 405 Method Not Allowed\r\n"
                 "Content-Type: text/plain\r\n"
                 "Content-Length: 18\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "Method Not Allowed");
        send(client_socket, response, strlen(response), 0);
        return;
    }

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
        send(client_socket, response, strlen(response), 0);
    } else {
        serve_static_file(url, client_socket);
    }
}
