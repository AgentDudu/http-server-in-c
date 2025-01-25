#include "static_handler.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <winsock2.h>

#define CACHE_SIZE 10

typedef struct {
  char path[256];
  char *content;
  size_t size;
  time_t last_modified;
} FileCache;

static FileCache cache[CACHE_SIZE];
static int cache_count = 0;

FileCache *find_in_cache(const char *path) {
  for (int i = 0; i < cache_count; i++) {
    if (strcmp(cache[i].path, path) == 0) {
      return &cache[i];
    }
  }
  return NULL;
}

void update_cache(const char *path, const char *content, size_t size,
                  time_t last_modified) {
  FileCache *entry = find_in_cache(path);

  if (entry == NULL) {
    if (cache_count < CACHE_SIZE) {
      entry = &cache[cache_count++];
    } else {
      entry = &cache[0];
    }
    strncpy(entry->path, path, sizeof(entry->path) - 1);
  }

  free(entry->content);
  entry->content = malloc(size);
  if (entry->content != NULL) {
    memcpy(entry->content, content, size);
  }
  entry->size = size;
  entry->last_modified = last_modified;
}

int validate_url(const char *url) {
  // Reject if URL contains "../"
  if (strstr(url, "../") != NULL) {
    return 0;
  }

  // Reject if URL starts with "/../" or "/"
  if (url[0] == '/') {
    const char *clean_path = url + 1;
    if (strstr(clean_path, "../") == clean_path) {
      return 0;
    }
  }

  return 1;
}

void serve_static_file(const char *url, SOCKET client_socket) {
  char path[256];
  char response[2048];
  char *body;
  size_t body_length;
  FILE *file;
  struct stat st;

  // Validate the URL for directory traversal
  if (!validate_url(url)) {
    log_error("Directory traversal attempt detected.");
    snprintf(response, sizeof(response),
             "HTTP/1.1 403 Forbidden\r\n"
             "Content-Type: text/plain\r\n"
             "Content-Length: 9\r\n"
             "Connection: close\r\n"
             "\r\n"
             "Forbidden");
    send(client_socket, response, strlen(response), 0);
    return;
  }

  snprintf(path, sizeof(path), "static%s",
           strcmp(url, "/") == 0 ? "/index.html" : url);

  FileCache *cached = find_in_cache(path);
  if (cached != NULL && stat(path, &st) == 0 &&
      cached->last_modified == st.st_mtime) {
    // Serve from cache
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %zu\r\n"
             "Connection: close\r\n"
             "\r\n",
             cached->size);
    send(client_socket, response, strlen(response), 0);
    send(client_socket, cached->content, cached->size, 0);
    return;
  }

  // Read from disk and update cache
  if (stat(path, &st) != 0 || (file = fopen(path, "rb")) == NULL) {
    char error_message[512];
    snprintf(error_message, sizeof(error_message), "File not found: %s", path);
    log_error(error_message);

    snprintf(response, sizeof(response),
             "HTTP/1.1 404 Not Found\r\n"
             "Content-Type: text/plain\r\n"
             "Content-Length: 9\r\n"
             "Connection: close\r\n"
             "\r\n"
             "Not Found");
    send(client_socket, response, strlen(response), 0);
    return;
  }

  body = malloc(st.st_size);
  if (body == NULL) {
    fclose(file);
    log_error("Memory allocation failed.");
    snprintf(response, sizeof(response),
             "HTTP/1.1 500 Internal Server Error\r\n"
             "Content-Type: text/plain\r\n"
             "Content-Length: 21\r\n"
             "Connection: close\r\n"
             "\r\n"
             "Internal Server Error");
    send(client_socket, response, strlen(response), 0);
    return;
  }

  body_length = fread(body, 1, st.st_size, file);
  fclose(file);

  update_cache(path, body, body_length, st.st_mtime);

  snprintf(response, sizeof(response),
           "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/html\r\n"
           "Content-Length: %zu\r\n"
           "Connection: close\r\n"
           "\r\n",
           body_length);

  send(client_socket, response, strlen(response), 0);
  send(client_socket, body, body_length, 0);

  free(body);
}
