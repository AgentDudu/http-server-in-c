#ifndef SESSION_H
#define SESSION_H

#include <stddef.h>

typedef struct {
  char session_id[37];
  char data[1024];
} Session;

void init_session_store();
Session *get_session(const char *session_id);
Session *create_session();
void delete_session(const char *session_id);

#endif
