#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include <stddef.h>

int
main(int argc, char *argv[])
{
  if ( argc < 2) {
  	printf("usage: split <filename>\n");
  	return -1;
  }

  char *file_name = argv[1];

  int fd = open(file_name, O_RDONLY);
  if (fd < 0) {
    printf("cannot open %s\n", file_name);
    return -1;
  }
  
  uint sz = 128;
  split(fd, sz);

  close(fd);
  return 0;
}

void
split(int fd, uint sz)
{
  char buffer[sz];
  int chars_read;
  uint word_sz = sz;
  char *word = malloc(word_sz);
  int word_index = 0;

  if(word == NULL) {
    printf("malloc failed\n");
    exit(1);
  }

  while ((chars_read = read(fd, buffer, sz)) > 0) {
    for (int i = 0; i < chars_read; ++i) {
      char c = buffer[i];

      if (c == ' ' || c == '\n' || c == '\t') {
      	if (word_index > 0) {
      	  word[word_index] = '\0';
      	  printf("%s\n", word);
      	  word_index = 0;
      	}
      } else {
      	  word[word_index++] = c;

      	  if (word_index >= word_sz - 1) {
      	  	int new_sz = word_sz * 2;
      	  	char *new_word = malloc(new_sz);
      	  	if (new_word) {
      	  	  printf("malloc failed\n");
      	  	  free(word);
      	  	  return;
      	  	}

      	  	memcpy(new_word, word, word_sz);
      	  	free(word);
      	  	word = new_word;
      	  	word_sz = new_sz;
      	  }
      }
    }
  }
  
  if (word_index > 0) {
    word[word_index] = '\0';
    printf("%s\n", word);
  }

  free(word);
}
