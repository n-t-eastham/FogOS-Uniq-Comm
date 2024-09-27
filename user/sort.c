#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include <stddef.h>

int
main(int argc, char *argv[])
{
  if (argc < 2) {
  	printf("usage: sort <filename> [-l | -w]\n");
  	return -1;
  }

  char *file_name = argv[1];

  int fd = open(file_name, O_RDONLY);
  if (fd < 0) {
  	printf("cannot open %s\n", file_name);
  	return -1;
  }

  /*
  * Currently the second and third args of sort are 
  * meant for flags, but we are ommiting those for now.
  */
  if (sort(fd, 0, NULL) != 0) {
  	printf("sort failed\n");
  	close(fd);
  	return 1;
  }

  return 0;
}


void
free_lines(int num_lines, char *lines[])
{
  for (int i = 0; i < num_lines; i++) {
    free(*(lines + i));
  }
  free(lines);
  lines = NULL;
}

void
print_lines(int num_lines, char *lines[])
{
  for (int i = 0; i < num_lines; i++) {
  	printf("%s", *(lines + i));
  }
}

void
insertion_sort_line(int num_lines, char *lines[])
{
  for (int i = 1; i < num_lines; i++) {
  	char *curr_line = lines[i];
  	int j = i - 1;

  	while (j >= 0 && strcmp(lines[j], curr_line) > 0) {
  	  lines[j + 1] = lines[j];
  	  j--;
  	}

  	lines[j + 1] = curr_line;
  }
}


int 
sort(int fd, int num_flags, char *flags[]) 
{
  uint sz = 128;
  int count = 0; 
  uint buf = 0;  
  char **lines = 0;

 
  while (1) {
    
    if (count >= buf) {
      uint new_buf;
      if (buf == 0) {
        new_buf = 8;
      } else {
        new_buf = buf * 2;
      }

      char **new_lines = (char **)malloc(new_buf * sizeof(char *));
      if (new_lines == NULL) {
        printf("malloc failed\n");
        return 1;
      }

      if (lines) {
        for (int i = 0; i < count; i++) {
          new_lines[i] = lines[i];
        }
        free(lines);
      }

      lines = new_lines;
      buf = new_buf;
    }

    char *line = malloc(sz);
    if (line == NULL) {
      printf("malloc failed\n");
      return 1;
    }
    
    if (getline(&line, &sz, fd) <= 0) {
      free(line);
      break;
    }

    lines[count] = line;
    count++;
  }
    
    insertion_sort_line(count, lines);
    print_lines(count, lines);
    free_lines(count, lines);

    return 0;
}
