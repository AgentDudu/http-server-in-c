#include "method_handler.h"
#include "logger.h"
#include "router.h" // Now includes send_response declaration
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

void handle_method_request(const char *method, const char *url,
                           const char *request, SOCKET client_socket) {
  char response_body[1024] = {0};

  if (strcmp(method, "POST") == 0) {
    snprintf(response_body, sizeof(response_body),
             "POST request received for %s", url);
    send_response(client_socket, "201 Created", "text/plain", response_body,
                  NULL);
  } else if (strcmp(method, "PUT") == 0) {
    snprintf(response_body, sizeof(response_body),
             "PUT request received for %s", url);
    send_response(client_socket, "200 OK", "text/plain", response_body, NULL);
  } else if (strcmp(method, "DELETE") == 0) {
    snprintf(response_body, sizeof(response_body),
             "DELETE request received for %s", url);
    send_response(client_socket, "200 OK", "text/plain", response_body, NULL);
  } else {
    snprintf(response_body, sizeof(response_body), "Method %s Not Supported",
             method);
    send_response(client_socket, "405 Method Not Allowed", "text/plain",
                  response_body, NULL);
  }
}
