#ifndef STATIC_HANDLER_H
#define STATIC_HANDLER_H

#include <winsock2.h>

void serve_static_file(const char *url, SOCKET client_socket);

#endif
