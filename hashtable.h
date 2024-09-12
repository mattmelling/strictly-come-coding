#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "parser.h"

struct ht_entry {
  char *name;
  double min;
  double max;
  double avg;
  double tot;
  int n;
};

struct hashtable {
  unsigned long size;
  struct ht_entry *entries;
};

struct hashtable * ht_create(unsigned long size);
void ht_free(struct hashtable *ht);
void ht_insert_reading(struct hashtable *ht, struct reading *r);
void ht_insert(struct hashtable *ht, struct ht_entry *entry);
struct ht_entry * ht_get(struct hashtable *ht, char *key);
unsigned long ht_count(struct hashtable *ht);

#endif
