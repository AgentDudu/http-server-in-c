#include "content_negotiation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char media_type[50];
  float q;
} AcceptEntry;

void parse_accept_header(const char *accept_header, AcceptEntry *entries,
                         int *count) {
  *count = 0;
  char buffer[256];
  strncpy(buffer, accept_header, sizeof(buffer) - 1);
  buffer[sizeof(buffer) - 1] = '\0';

  char *token = strtok(buffer, ",");
  while (token && *count < MAX_ACCEPT_TYPES) {
    char *q_value = strstr(token, ";q=");
    float q = 1.0f;

    if (q_value) {
      *q_value = '\0';
      q_value += 3;
      q = strtof(q_value, NULL);
    }

    strncpy(entries[*count].media_type, token,
            sizeof(entries[*count].media_type) - 1);
    entries[*count].media_type[sizeof(entries[*count].media_type) - 1] = '\0';
    entries[*count].q = q;
    (*count)++;

    token = strtok(NULL, ",");
  }
}

int compare_q_values(const void *a, const void *b) {
  float q1 = ((AcceptEntry *)a)->q;
  float q2 = ((AcceptEntry *)b)->q;
  return (q1 < q2) - (q1 > q2);
}

const char *get_best_content_type(const char *accept_header) {
  static AcceptEntry entries[MAX_ACCEPT_TYPES];
  int count = 0;

  parse_accept_header(accept_header, entries, &count);
  qsort(entries, count, sizeof(AcceptEntry), compare_q_values);

  for (int i = 0; i < count; i++) {
    if (strstr(entries[i].media_type, "application/json"))
      return "application/json";
    if (strstr(entries[i].media_type, "application/xml"))
      return "application/xml";
    if (strstr(entries[i].media_type, "text/html"))
      return "text/html";
  }

  return "text/html";
}
