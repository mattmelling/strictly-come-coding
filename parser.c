#include "debug.h"
#include "parser.h"

char * parse_read_field(char *buf, char *end, char delim)
{
  char *p = buf;
  while (p < end) {
    if (*p == delim)
      return p;
    p++;
  }
  return NULL;
}

char * parse_reading(char *buf, char *end, struct reading *reading)
{
  if (buf >= end)
    return NULL;

  char *p = parse_read_field(buf, end, ';');
  if (p == NULL)
    return NULL;

  size_t len = p - buf;
  memcpy(reading->location, buf, len);
  reading->location[len] = NULL;

  p++;

  if (buf >= end)
    return NULL;

  reading->reading = strtod(p, NULL);
  p = parse_read_field(p, end, '\n');
  if (p == NULL)
    return NULL;

  return p + 1;
}
