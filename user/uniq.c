#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include <stdbool.h>
#include <stddef.h>


/*
* writes a line word by word to a given file
*
* @param line to write
* @param fd file descriptor to write to
*/
void *
writewords(char *line, int fd)
{
  // first char we check is the start of the line
  char *character = &line[0];
  while (*character != '\0') { // while we aren't at the end
    if (*character == ' ' || *character == '\t') { // if we reach the end of a word
      write(fd, "\n", 1);
    }
    else {
      write(fd, character, 1);
    }
    character++;
  }

  return 0;
}


/*
* prints out the uniq instances from a provided sorted
* char* array
*
* @param num_lines max num of lines
* @param lines array of lines
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


/*
* prints out the uniq instances from a provided sorted
* char* array
*
* @param count max num of lines or words
* @param lines array of lines or words
* @param cflag boolean for the -c flag
*/
void 
print_uniq(int count, char *lines[], bool cflag) 
{
  int curr = 0; // keeps track of which line we're at
  int instances = 1; // keeps track of how many instances

  while(curr < count) { // while we still have stuff to read
    // skip null terms and line ends
    if ((strcmp(lines[curr], "\0") == 0) || (strcmp(lines[curr], "\n") == 0)
            || (strcmp(lines[curr], "\t") == 0)) {
       continue;
    }

    if((curr + 1 < count) && strcmp(lines[curr], lines[curr + 1]) == 0) {
      instances++;
    }
    // if we have hit the last uniq instance, print it out
    else {
      if (cflag) { // with count
        printf("%d %s", instances, lines[curr]);
      }
      else { // without count
        printf("%s", lines[curr]);
      }
      instances = 1;
    }

    curr++;
  }
}


/*
* creates a char * of all the lines from a given file and
* prints out all the unique lines
*
* @param fd file descriptor of file to read
* @param cflag boolean for the -c flag
*/
void 
lines(int fd, bool cflag) 
{
  uint sz = 20; // getline will resize if necessary
  int buf = 0;
  int count = 0; // num of lines
  char **lines = 0;
  
  while (1) {
    if (count >= buf) {
    uint new_buf;
    if (buf == 0) {
       new_buf = 8; // small buf to see reallocation
    } else {
      new_buf = buf * 2;
    }

    char **new_lines = malloc(new_buf * sizeof(char *));
    if (new_lines == 0) {
      printf("malloc failed");
      exit(1);
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
    if (getline(&line, &sz, fd) <= 0) {
      free(line);
      break;
    }
      
    lines[count] = line;
    count++;
  }
  print_uniq(count, lines, cflag);
      
  for (int i = 0; i < count; i++) {
    free(lines[i]);
  }
}


/*
* creates a char * of all the lines from a given file and
* prints out all the unique lines
*
* @param argv array of args to check for flags
* @param fd file descriptor of file to read
*/
void
uniq(char *argv[], int fd) 
{
  int j = 0; 

  //FINDING PROPER FLAGS
  while (argv[j] != NULL) {
    if (!strcmp(argv[j], "-c")) {
      lines(fd, true);
      return;
    }
    j++;
  }
  
  lines(fd, false);
}


int main(int argc, char *argv[])
{
  int fd = 0;
  int arg_start = 1;

  if (argc > 1 && argv[1][0] != '-') {
  	fd = open(argv[1], O_RDONLY);
  	if (fd < 0) {
    printf("cannot open file %s\n", argv[1]);
	return -1;
    }
   arg_start = 2;
  }
  
  uniq(&argv[arg_start], fd);

  if (fd != 0) {
  	close(fd);
  }
  return 0;
}
