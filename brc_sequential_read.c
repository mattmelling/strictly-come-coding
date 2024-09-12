#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "hashtable.h"
#include "parser.h"
#include "debug.h"

#define BUFFER_SIZE 64 * 1048576
#define TABLE_SIZE 1000000

unsigned long read_entries(char *buf, unsigned long size, struct hashtable *ht)
{
  struct reading r;
  char location[64];
  r.location = &location;
  memset(&location, 0, 64);

  char *end = buf + size;
  char *p = buf;
  char *l = p;
  while(p != NULL) {
    p = parse_reading(p, end, &r);
    if (p == NULL)
      break;
    l = p;
    ht_insert_reading(ht, &r);
  }

  return (buf + size) - l;
}

static int cmpstr(const void *p1, const void *p2)
{
  return strcmp(*(const char **)p1, *(const char **)p2);
}

void main(int argc, char **argv)
{

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

  unsigned long size = sb.st_size;
  unsigned long offset = 0;

  struct hashtable *ht = ht_create(TABLE_SIZE);
  char *buf = malloc(sizeof(char) * BUFFER_SIZE);
  ssize_t n;
  int d = 0;

  while (offset < size) {
    lseek(fd, offset, SEEK_SET);
    n = read(fd, buf, BUFFER_SIZE);
    d = read_entries(buf, n, ht);
    offset = offset + n - d;
  }

  int n_items = ht_count(ht);
  int j = 0;
  char **items = malloc(sizeof(char *) * n_items);
  struct ht_entry *e;
  for(int i = 0; i < ht->size; i++) {
    if (ht->entries[i].name != NULL) {
      e = &ht->entries[i];
      items[j] = malloc(sizeof(char) * 64);
      sprintf(items[j], "%s=%.1f/%.1f/%.1f", e->name, e->min, e->avg, e->max);
      n++;
      j++;
    }
  }

  qsort(items, n_items, sizeof(char *), cmpstr);

  FILE *fo = fopen("output.txt", "w");
  for(int i = 0; i < n_items; i++) {
    fprintf(fo, "%s\n", items[i]);
  }
  fclose(fo);
}
