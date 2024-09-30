#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include <stddef.h>

int
main(int argc, char *argv[])
{
  int fd;
  
  if (argc == 1) {
  	fd = 0;
  } else if (argc >= 2) {
      char * file_name = argv[1];
      fd = open(file_name, O_RDONLY);
      if (fd < 0) {
        printf("cannot open %s\n", file_name);
        return -1;
      }
  } else {
  	return -1;
  }


  /**
   * Currently the second and third args of sort are 
   * meant for flags, but we are ommiting those for now.
   */
  if (sort(fd, 0, NULL) != 0) {
  	printf("sort failed\n");
  	if (fd != 0) close(fd);
  	return 1;
  }

  if (fd != 0) close(fd);
  return 0;
}


/**
 * frees the memory allocated for an array of strings
 * 
 * @param num_lines number of lines in the array
 * @param lines array of strings to be freed
 */
void
free_lines(int num_lines, char *lines[])
{
  for (int i = 0; i < num_lines; i++) {
    free(*(lines + i));
  }
  free(lines);
  lines = NULL;
}


/**
 * prints an array of strings
 * 
 * @param num_lines number of lines in the array
 * @param lines array of strings to be printed
*/
void
print_lines(int num_lines, char *lines[])
{
  for (int i = 0; i < num_lines; i++) {
  	printf("%s", *(lines + i));
  }
}


/**
 * sorts an array of strings using insertion sort
 * 
 * @param num_lines number of lines in the array
 * @param lines array of strings to be sorted
 */
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


/**
 * reads lines from a file, sorts them, and prints them
 * 
 * @param fd file descriptor of the file to be sorted
 * @param num_flags number of flags (currently unused)
 * @param flags array of flags (currently unused)
 * @return int returns 0 on success, 1 on failure
 */
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
