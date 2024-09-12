#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "debug.h"
#include "parser.h"
#include "hashtable.h"

void print_entry(struct ht_entry *entry)
{
  debug("ht_entry: %s min %f max %f avg %f \n", entry->name, entry->min, entry->max, entry->avg);
}

int main(int argc, char **argv)
{
  struct hashtable *ht = ht_create(10000);

  int fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    perror("open");
    exit(-1);
  }

  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    perror("fstat");
    exit(-1);
  }

  char *buf = malloc(sb.st_size + 1);
  read(fd, buf, sb.st_size);
  buf[sb.st_size] = '\0';
  close(fd);

  char location[32];
  struct reading r;
  r.location = &location;

  char *p = buf;
  char *end = buf + sb.st_size;
  while (p != NULL)
  {
    p = parse_reading(p, end, &r);
    if (p == NULL)
      break;
    ht_insert_reading(ht, &r);
  }
  free(buf);

  struct ht_entry entry;
  for(int i = 0; i < ht->size; i++) {
    entry = ht->entries[i];
    if (entry.name != NULL)
      print_entry(&entry);
  }

  ht_free(ht);
  exit(0);
}
