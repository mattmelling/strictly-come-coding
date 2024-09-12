#include "debug.h"
#include "hash.h"
#include "hashtable.h"

struct hashtable * ht_create(unsigned long size)
{
  struct hashtable *ht = malloc(sizeof(struct hashtable));
  unsigned long sz = sizeof(struct ht_entry) * size;
  ht->entries = malloc(sz);
  memset(ht->entries, NULL, sz);
  ht->size = size;
  return ht;
}

void ht_free(struct hashtable *ht)
{
  for(int i = 0; i < ht->size; i++) {
    if (ht->entries[i].name != NULL)
      free(ht->entries[i].name);
  }
  free(ht->entries);
  free(ht);
}

void ht_insert(struct hashtable *ht, struct ht_entry *entry)
{
  unsigned long h = djb2(entry->name) % ht->size;
  struct ht_entry *p = ht->entries + h;

  if (p->name == NULL) {
    long sz = strlen(entry->name);
    p->name = malloc(sizeof(char) * sz);
    memcpy(p->name, entry->name, sz);
    p->min = entry->min;
    p->max = entry->max;
    p->avg = entry->avg;
    p->tot = entry->tot;
    p->n = entry->n;
  } else {
    p->min = fmin(entry->min, p->min);
    p->max = fmax(entry->max, p->max);
    p->tot += entry->tot;
    p->n += entry->n;
    p->avg = p->tot / p->n;
  }
}

void ht_insert_reading(struct hashtable *ht, struct reading *r)
{
  unsigned long h = djb2(r->location) % ht->size;
  struct ht_entry *p = ht->entries + h;

  if (p->name == NULL) {
    long sz = strlen(r->location);
    p->name = malloc(1 + (sizeof(char) * sz));
    memcpy(p->name, r->location, sz + 1);
    p->name[sz] = NULL;
    p->min = r->reading;
    p->max = r->reading;
    p->avg = r->reading;
    p->tot = r->reading;
    p->n = 1;
  } else {
    p->min = fmin(r->reading, p->min);
    p->max = fmax(r->reading, p->max);
    p->tot += r->reading;
    p->n++;
    p->avg = p->tot / p->n;
  }
}

struct ht_entry * ht_get(struct hashtable *ht, char *key)
{
  unsigned long h = djb2(key) % ht->size;
  return ht->entries + h;
}

unsigned long ht_count(struct hashtable *ht)
{
  unsigned long n = 0;
  struct ht_entry *p;
  for(int i = 0; i < ht->size; i++) {
    p = ht->entries + i;
    if (p->name != NULL)
      n++;
  }
  return n;
}
