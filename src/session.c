#include "session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SESSIONS 100

static Session session_store[MAX_SESSIONS];
static int session_count = 0;

void generate_session_id(char *buffer, size_t size) {
  const char *chars =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  for (size_t i = 0; i < size - 1; i++) {
    buffer[i] = chars[rand() % strlen(chars)];
  }
  buffer[size - 1] = '\0';
}

void init_session_store() {
  srand((unsigned int)time(NULL));
  memset(session_store, 0, sizeof(session_store));
}

Session *get_session(const char *session_id) {
  for (int i = 0; i < session_count; i++) {
    if (strcmp(session_store[i].session_id, session_id) == 0) {
      return &session_store[i];
    }
  }
  return NULL;
}

Session *create_session() {
  if (session_count >= MAX_SESSIONS) {
    return NULL; // Session store full
  }

  Session *session = &session_store[session_count++];
  generate_session_id(session->session_id, sizeof(session->session_id));
  memset(session->data, 0, sizeof(session->data));
  return session;
}

void delete_session(const char *session_id) {
  for (int i = 0; i < session_count; i++) {
    if (strcmp(session_store[i].session_id, session_id) == 0) {
      for (int j = i; j < session_count - 1; j++) {
        session_store[j] = session_store[j + 1];
      }
      session_count--;
      return;
    }
  }
}
