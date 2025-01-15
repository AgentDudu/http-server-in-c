#ifndef ROUTER_H
#define ROUTER_H

#include <winsock2.h>

void handle_request(const char *request, SOCKET client_socket);

#endif
