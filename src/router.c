#include "router.h"
#include "logger.h"
#include "request.h"
#include "static_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

void send_response(SOCKET client_socket, const char *status,
                   const char *content_type, const char *body) {
  char response[2048];
  int body_length = strlen(body);

  snprintf(response, sizeof(response),
           "HTTP/1.1 %s\r\n"
           "Content-Type: %s\r\n"
           "Content-Length: %d\r\n"
           "Connection: close\r\n"
           "\r\n"
           "%s",
           status, content_type, body_length, body);

  send(client_socket, response, strlen(response), 0);
}

void handle_request(const char *request, SOCKET client_socket) {
  char method[16] = {0}, url[256] = {0}, protocol[16] = {0};
  char body[1024] = {0};

  if (sscanf(request, "%15s %255s %15s", method, url, protocol) != 3) {
    log_error("Malformed HTTP request.");
    send_response(client_socket, "400 Bad Request", "text/plain",
                  "Bad Request");
    return;
  }

  if (strcmp(protocol, "HTTP/1.1") != 0) {
    log_error("Unsupported protocol version.");
    send_response(client_socket, "505 HTTP Version Not Supported", "text/plain",
                  "HTTP Version Not Supported");
    return;
  }

  if (strcmp(method, "GET") != 0) {
    log_error("Unsupported HTTP method.");
    send_response(client_socket, "405 Method Not Allowed", "text/plain",
                  "Method Not Allowed");
    return;
  }

  log_request(method, url);

  // Static routes
  if (strcmp(url, "/") == 0) {
    snprintf(body, sizeof(body), "Hello World");
    send_response(client_socket, "200 OK", "text/plain", body);
  } else if (strcmp(url, "/about") == 0) {
    snprintf(body, sizeof(body), "About Page");
    send_response(client_socket, "200 OK", "text/plain", body);
  }
  // Dynamic route: /user/{id}
  else if (strncmp(url, "/user/", 6) == 0) {
    char *id = url + 6; // Extract the portion after /user/
    if (strlen(id) == 0 || strspn(id, "0123456789") != strlen(id)) {
      log_error("Invalid user ID in URL.");
      send_response(client_socket, "400 Bad Request", "text/plain",
                    "Invalid User ID");
      return;
    }

    snprintf(body, sizeof(body), "User ID: %s", id);
    send_response(client_socket, "200 OK", "text/plain", body);
  } else {
    // Handle static files or unknown routes
    serve_static_file(url, client_socket);
  }
}
