#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "hashtable.h"
#include "parser.h"
#include "debug.h"

#define BUFFER_SIZE 1000 * 1048576
#define TABLE_SIZE 1000000

unsigned long N = 0;

unsigned long read_entries(char *buf, char *end, struct hashtable *ht)
{
  struct reading r;
  char location[64];
  r.location = &location;
  memset(&location, 0, 64);

  char *ptr = buf;
  char *last_ptr = ptr;

  while(ptr < end) {
    last_ptr = ptr;
    ptr = parse_reading(ptr, end, &r);
    if (ptr == end)
      return ptr;
    if (ptr == NULL)
      break;
    N++;
    ht_insert_reading(ht, &r);
  }

  return last_ptr;
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

  struct hashtable *ht = ht_create(TABLE_SIZE);

  unsigned long file_size = sb.st_size;
  unsigned long file_offset = 0;
  unsigned long page_offset = 0;
  unsigned long inner_offset = 0;
  unsigned long page_size = 0;

  char *buf;
  char *ptr;
  char *end;
  unsigned long len = 0;

  while (file_offset < file_size) {
    page_size = BUFFER_SIZE;

    if (file_offset > file_size) {
      break;
    }

    page_offset = file_offset & ~(sysconf(_SC_PAGE_SIZE) - 1);
    inner_offset = file_offset - page_offset;

    // make sure we don't map past the end of the file
    if ((page_offset + page_size) > file_size) {
      page_size = file_size - file_offset + inner_offset;
    } else {
      page_size += inner_offset;
    }

    /* debug("file_offset %x page_size %x, page_offset %x end %p\n", file_offset, page_size, page_offset, buf + page_size); */
    buf = mmap(NULL, page_size, PROT_READ, MAP_PRIVATE, fd, page_offset);
    if (buf == MAP_FAILED) {
      perror("mmap");
      exit(-1);
    }

    if (madvise(buf, page_size, MADV_SEQUENTIAL | MADV_WILLNEED) != 0) {
      perror("madvise");
      exit(-1);
    }

    ptr = buf + inner_offset;
    end = buf + page_size;
    end = read_entries(ptr, end, ht);
    len = end - ptr;
    file_offset += len;

    if (munmap(buf, page_size) != 0) {
      perror("munmap");
      exit(-1);
    }
  }

  int n = 0;
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
  debug("%u\n", N);

  qsort(items, n_items, sizeof(char *), cmpstr);

  FILE *fo = fopen("output.txt", "w");
  for(int i = 0; i < n_items; i++) {
    fprintf(fo, "%s\n", items[i]);
  }
  fclose(fo);
}
