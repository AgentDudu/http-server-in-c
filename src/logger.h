#ifndef LOGGER_H
#define LOGGER_H

#include <winsock2.h>

void log_message(const char *level, const char *message);
void log_request(const char *method, const char *url, const char *client_ip, double duration);
void log_error(const char *error);
void rotate_logs();

#endif
