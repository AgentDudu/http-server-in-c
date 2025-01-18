#include "logger.h"
#include <stdio.h>
#include <time.h>

void log_message(const char *level, const char *message) {
    FILE *logfile = fopen("server.log", "a");
    if (logfile == NULL) {
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    fprintf(logfile, "[%02d-%02d-%02d %02d:%02d:%02d] [%s] %s\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec, level, message);

    fclose(logfile);

    printf("[%s] %s\n", level, message);
}

void log_request(const char *method, const char *url) {
    char log_entry[512];
    snprintf(log_entry, sizeof(log_entry), "Request: Method=%s, URL=%s", method, url);
    log_message("INFO", log_entry);
}

void log_error(const char *error) {
    log_message("ERROR", error);
}
