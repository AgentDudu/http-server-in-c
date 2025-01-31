#ifndef CONTENT_NEGOTIATION_H
#define CONTENT_NEGOTIATION_H

#define MAX_ACCEPT_TYPES 10

const char *get_best_content_type(const char *accept_header);

#endif
