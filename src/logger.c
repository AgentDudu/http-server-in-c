#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define LOG_FILE "server.log"
#define LOG_BACKUP "server_old.log"
#define MAX_LOG_SIZE 1024 * 1024  // 1MB log rotation threshold

void log_message(const char *level, const char *message) {
    rotate_logs(); // Check log size and rotate if necessary

    FILE *logfile = fopen(LOG_FILE, "a");
    if (logfile == NULL) {
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    fprintf(logfile, "[%02d-%02d-%02d %02d:%02d:%02d] [%s] %s\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec, level, message);
    fclose(logfile);
}

// Log requests with client IP and processing time
void log_request(const char *method, const char *url, const char *client_ip, double duration) {
    char log_entry[512];
    snprintf(log_entry, sizeof(log_entry), "Request: Method=%s, URL=%s, IP=%s, Duration=%.3fms",
             method, url, client_ip, duration);
    log_message("INFO", log_entry);
}

// Log errors
void log_error(const char *error) {
    log_message("ERROR", error);
}

// Log Rotation: Archives old logs when size exceeds limit
void rotate_logs() {
    struct stat log_stat;
    if (stat(LOG_FILE, &log_stat) == 0 && log_stat.st_size > MAX_LOG_SIZE) {
        remove(LOG_BACKUP);
        rename(LOG_FILE, LOG_BACKUP);
    }
}
