#ifndef CONTENT_NEGOTIATION_H
#define CONTENT_NEGOTIATION_H
#include <stddef.h>

#define MAX_ACCEPT_TYPES 10

const char *get_best_content_type(const char *accept_header);
void format_response(char *body, size_t size, const char *content_type,
                     const char *session_id);

#endif
