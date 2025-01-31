#include "response_handler.h"
#include <stdio.h>
#include <string.h>
#include <winsock2.h>

void send_response(SOCKET client_socket, const char *status,
                   const char *content_type, const char *body,
                   const char *set_cookie) {
    char response[4096];
    int body_length = strlen(body);

    if (set_cookie) {
        snprintf(response, sizeof(response),
                 "HTTP/1.1 %s\r\n"
                 "Content-Type: %s\r\n"
                 "Content-Length: %d\r\n"
                 "Set-Cookie: session_id=%s\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "%s",
                 status, content_type, body_length, set_cookie, body);
    } else {
        snprintf(response, sizeof(response),
                 "HTTP/1.1 %s\r\n"
                 "Content-Type: %s\r\n"
                 "Content-Length: %d\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "%s",
                 status, content_type, body_length, body);
    }

    send(client_socket, response, strlen(response), 0);
}
