#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include <stdbool.h>
#include <stddef.h>

//
// wrapper so that it's OK if main() does not call exit().
//
void
_main()
{
  extern int main();
  main();
  exit(0);
}

char*
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  char *cdst = (char *) dst;
  int i;
  for(i = 0; i < n; i++){
    cdst[i] = c;
  }
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  fgets(0, buf, max);
  return buf;
}

int
fgets(int fd, char *buf, int max)
{
  int i = 0, cc;
  char c;
  while(i < max -1) {
    cc = read(fd, &c, 1);
    if (cc < 1) { break; }
    buf[i++] = c;
    if (c == '\n' || c == '\r') { break; }
  }
  
  buf[i] = '\0';
  return i;
}

int
getline(char **lineptr, uint *n, int fd)
{
  //allocate a buffer if *lineptr == NULL and n = 0
  if (*lineptr == NULL && *n == 0) {
    *n = 2;
    *lineptr = malloc(*n);
    if (*lineptr == NULL) {
      return -1;
    }
  }
  int total = 0;
  
  while (true) {
    //with each iteration, pointer arithmetic is used to traverse the buffer
    int bytes_read = fgets(fd, *lineptr + total, *n - total);
    
    if (bytes_read < 0) {
      return -1;
    } else if (bytes_read == 0) {
      return total;
    }
    total += bytes_read;
    if ((*lineptr)[total - 1] == '\n') { break; }
    //double the buffer size
    uint resize = *n << 1;
    char *new_buf = malloc(resize);
    if (new_buf == NULL) {
      return -1;
    }
    memcpy(new_buf, *lineptr, *n);
    free(*lineptr);
    
    *lineptr = new_buf;
    *n = resize; 
  }
  return total;
}


int
stat(const char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, const void *vsrc, int n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  if (src > dst) {
    while(n-- > 0)
      *dst++ = *src++;
  } else {
    dst += n;
    src += n;
    while(n-- > 0)
      *--dst = *--src;
  }
  return vdst;
}

int
memcmp(const void *s1, const void *s2, uint n)
{
  const char *p1 = s1, *p2 = s2;
  while (n-- > 0) {
    if (*p1 != *p2) {
      return *p1 - *p2;
    }
    p1++;
    p2++;
  }
  return 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  return memmove(dst, src, n);
}

