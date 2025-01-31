#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <winsock2.h>

void send_response(SOCKET client_socket, const char *status, const char *content_type,
                   const char *body, const char *set_cookie);

#endif
