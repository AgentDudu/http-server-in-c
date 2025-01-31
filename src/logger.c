#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define LOG_FILE "server.log"
#define MAX_LOG_FILES 5
#define MAX_LOG_SIZE 1024 * 1024 // 1MB log rotation threshold

void log_message(const char *level, const char *message) {
  rotate_logs(); // Check log size and rotate if necessary

  FILE *logfile = fopen(LOG_FILE, "a");
  if (logfile == NULL) {
    return;
  }

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  fprintf(logfile, "[%02d-%02d-%02d %02d:%02d:%02d] [%s] %s\n",
          t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min,
          t->tm_sec, level, message);
  fclose(logfile);
}

// Log requests with client IP and processing time
void log_request(const char *method, const char *url, const char *client_ip,
                 double duration) {
  char log_entry[512];
  snprintf(log_entry, sizeof(log_entry),
           "Request: Method=%s, URL=%s, IP=%s, Duration=%.3fms", method, url,
           client_ip, duration);
  log_message("INFO", log_entry);
}

// Log errors
void log_error(const char *error) { log_message("ERROR", error); }

// Log Rotation: Archives old logs when size exceeds limit
void rotate_logs() {
  struct stat log_stat;
  if (stat(LOG_FILE, &log_stat) == 0 && log_stat.st_size > MAX_LOG_SIZE) {
    // Remove oldest log
    char oldest_log[32];
    snprintf(oldest_log, sizeof(oldest_log), "%s.%d", LOG_FILE, MAX_LOG_FILES);
    remove(oldest_log);

    // Shift log files (server.log.4 → server.log.5, etc.)
    for (int i = MAX_LOG_FILES - 1; i > 0; i--) {
      char old_log[32], new_log[32];
      snprintf(old_log, sizeof(old_log), "%s.%d", LOG_FILE, i);
      snprintf(new_log, sizeof(new_log), "%s.%d", LOG_FILE, i + 1);
      rename(old_log, new_log);
    }

    // Rename current log to server.log.1
    char backup_log[32];
    snprintf(backup_log, sizeof(backup_log), "%s.1", LOG_FILE);
    rename(LOG_FILE, backup_log);
  }
}
