#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "debug.h"
#include "parser.h"

void main(int argc, char **argv)
{
  unsigned long N = 0;
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

  struct reading r;
  r.location = malloc(32);

  char *p = buf;
  char *end = buf + sb.st_size;
  while (p != NULL)
  {
    p = parse_reading(p, end, &r);
    if (p == NULL)
      break;
    debug("%-32s %.1f\n", r.location, r.reading);
    N++;
  }

  free(buf);
  printf("Total items: %u\n", N);
  exit(0);
}
