#include "router.h"
#include "logger.h"
#include "request.h"
#include "session.h"
#include "static_handler.h"
#include "content_negotiation.h"
#include <stdio.h>
#include <stdlib.h>
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

void format_response(char *body, size_t size, const char *content_type,
                     const char *session_id) {
    if (strcmp(content_type, "application/json") == 0) {
        snprintf(body, size, "{ \"message\": \"Hello\", \"session_id\": \"%s\" }", session_id);
    } else if (strcmp(content_type, "application/xml") == 0) {
        snprintf(body, size, "<response><message>Hello</message><session_id>%s</session_id></response>", session_id);
    } else {
        snprintf(body, size, "<html><body><h1>Hello</h1><p>Session ID: %s</p></body></html>", session_id);
    }
}

void handle_request(const char *request, SOCKET client_socket) {
    char method[16] = {0}, url[256] = {0}, protocol[16] = {0};
    char accept_header[256] = "text/html";
    char session_id[37] = {0};
    char body[1024] = {0};
    int is_new_session = 0;

    if (sscanf(request, "%15s %255s %15s", method, url, protocol) != 3) {
        log_error("Malformed HTTP request.");
        send_response(client_socket, "400 Bad Request", "text/plain", "Bad Request", NULL);
        return;
    }

    if (strcmp(protocol, "HTTP/1.1") != 0) {
        log_error("Unsupported protocol version.");
        send_response(client_socket, "505 HTTP Version Not Supported", "text/plain", "HTTP Version Not Supported", NULL);
        return;
    }

    if (strcmp(method, "GET") != 0) {
        log_error("Unsupported HTTP method.");
        send_response(client_socket, "405 Method Not Allowed", "text/plain", "Method Not Allowed", NULL);
        return;
    }

    log_request(method, url);

    // Extract Accept header
    char *accept_pos = strstr(request, "Accept: ");
    if (accept_pos) {
        sscanf(accept_pos, "Accept: %255[^\r\n]", accept_header);
    }

    const char *content_type = get_best_content_type(accept_header);

    // Extract session ID from the Cookie header
    if (sscanf(request, "%*[^C]Cookie: session_id=%36s", session_id) == 1) {
        log_request("Cookie detected", session_id);
    }

    Session *session = get_session(session_id);
    if (session == NULL) {
        session = create_session();
        if (session == NULL) {
            send_response(client_socket, "500 Internal Server Error", "text/plain", "Session creation failed", NULL);
            return;
        }
        is_new_session = 1;
    }

    format_response(body, sizeof(body), content_type, session->session_id);

    if (is_new_session) {
        send_response(client_socket, "200 OK", content_type, body, session->session_id);
    } else {
        send_response(client_socket, "200 OK", content_type, body, NULL);
    }
}
