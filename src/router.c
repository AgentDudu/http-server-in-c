#include "router.h"
#include "content_negotiation.h"
#include "logger.h"
#include "method_handler.h"
#include "request.h"
#include "session.h"
#include "static_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

void handle_request(const char *request, SOCKET client_socket) {
  char method[16] = {0}, url[256] = {0}, protocol[16] = {0};
  char session_id[37] = {0};
  char body[1024] = {0};
  int is_new_session = 0;

  if (sscanf(request, "%15s %255s %15s", method, url, protocol) != 3) {
    log_error("Malformed HTTP request.");
    send_response(client_socket, "400 Bad Request", "text/plain", "Bad Request",
                  NULL);
    return;
  }

  if (strcmp(protocol, "HTTP/1.1") != 0) {
    log_error("Unsupported protocol version.");
    send_response(client_socket, "505 HTTP Version Not Supported", "text/plain",
                  "HTTP Version Not Supported", NULL);
    return;
  }

  log_request(method, url);

  // Handle methods beyond GET
  if (strcmp(method, "GET") != 0) {
    handle_method_request(method, url, request, client_socket);
    return;
  }

  char accept_header[256] = "text/html"; // Default

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
      send_response(client_socket, "500 Internal Server Error", "text/plain",
                    "Session creation failed", NULL);
      return;
    }
    is_new_session = 1;
  }

  // Format response based on content type
  format_response(body, sizeof(body), content_type, session->session_id);

  if (is_new_session) {
    send_response(client_socket, "200 OK", content_type, body,
                  session->session_id);
  } else {
    send_response(client_socket, "200 OK", content_type, body, NULL);
  }
}
