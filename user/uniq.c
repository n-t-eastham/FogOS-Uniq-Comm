#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include <stdbool.h>
#include <stddef.h>


/*
 * converts an uppercase character to lowercase
 *
 * @param c the character to be converted
 */
char 
tolower(char c) 
{
	if (c >= 'A' && c <= 'Z') {
	  return c + ('a' - 'A');
	}
	return c;
}


/*
 * performs a comparison between two strings
 * 
 * @param s1 the first string to be compared
 * @param s2 the second string to be compared
 */
int
strcmp_helper(const char *s1, const char *s2) {
  while (*s1 && *s2) {
    char c1 = tolower(*s1);
    char c2 = tolower(*s2);
    if (c1 != c2) {
      return c1 - c2;
    }
    s1++;
    s2++;
  }
  return tolower(*s1) - tolower(*s2);
}


/*
 * creates a char * of all the lines from a given file and
 * prints out all the unique lines
 *
 * @param fd file descriptor of file to read
 * @param cflag boolean for the -c flag
 */
void 
lines(int fd, bool cflag, bool tflag, bool fflag) 
{
  uint sz = 0;
  int dupe_count = 1, unique_count = 0;
  char *last_line = NULL, *line;

  if (getline(&line, &sz, fd) > 0) {
  		last_line = malloc(strlen(line) + 1);
  		if (last_line) {
  			strcpy(last_line, line);
  		}

  		while (getline(&line, &sz, fd) > 0) {
  				bool equal;

  				if (fflag) {
  					equal = strcmp_helper(line, last_line) == 0;
  				} else {
  					equal = strcmp(line, last_line) == 0;
  				}
  				
  				if (equal) {
  					dupe_count++;
  				} else {
  					unique_count++;
  					if (cflag) {
  						printf("%d %s", dupe_count, last_line);
  					} else {
  						printf("%s", last_line);
  					}
  					free(last_line);
  					last_line = malloc(strlen(line) + 1);
  					if (last_line) {
  						strcpy(last_line, line);
  					}
  					dupe_count = 1;
  				}
  		}

  		unique_count++;
  		if (cflag) {
  			printf("%d %s", dupe_count, last_line);
  		} else {
  			printf("%s", last_line);
  		}

  		if (tflag) {
  			printf("Total unique lines/words: %d\n", unique_count);
  		}

  		free(last_line);
  }

  free(line);
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
  bool cflag = false;
  bool tflag = false;
  bool fflag = false; 

  while (argv[j] != NULL) {
    if (!strcmp(argv[j], "-c")) {
      cflag = true;
    } else if (!strcmp(argv[j], "-t")) {
    	tflag = true;
    } else if (!strcmp(argv[j], "-f")) {
    	fflag = true;
    }
    j++;
  }
  
  lines(fd, cflag, tflag, fflag);
}


int 
main(int argc, char *argv[])
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
