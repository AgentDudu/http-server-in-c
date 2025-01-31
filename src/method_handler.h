#ifndef METHOD_HANDLER_H
#define METHOD_HANDLER_H

#include <winsock2.h>

void handle_method_request(const char *method, const char *url, const char *request, SOCKET client_socket);

void send_response(SOCKET client_socket, const char *status, const char *content_type, const char *body, const char *set_cookie);

#endif
