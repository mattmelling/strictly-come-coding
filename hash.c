unsigned long djb2(unsigned char *str)
{
  unsigned long hash = 0x1505;
  int c;

  while (c = *str++)
    hash = (hash << 5) + hash + c;

  return hash;
}
