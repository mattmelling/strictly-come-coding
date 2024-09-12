#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct reading {
  char *location;
  double reading;
};

char * parse_reading(char *buf, char *end, struct reading *reading);

#endif
