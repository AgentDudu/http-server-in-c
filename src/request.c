#include "request.h"
#include <stdio.h>

void parse_request(const char *request, char *method, char *url) {
  sscanf(request, "%s %s", method, url);
}
