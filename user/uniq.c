#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include <stdbool.h>
#include <stddef.h>

/*
* helper function for sort that swaps two
* values from an array
*
* @param array to swap in
* @param i one index to swap
* @param j other index to swap
*/
void
swap(char * list[], int i, int j) {
  char * temp = list[i];
  list[i] = list[j];
  list[j] = temp;
}

/*
* bubble sorts an array of char *
*
* @param array to sort
* @param length of array
*/
void
bubble_sort(char *list[], int length) {
  for (int i = 0; i < length; i++) {
    for (int j = i + 1; j < length; j++) {
      if (strcmp(list[i], list[j]) > 0) {
        swap(list, i, j);
      }
    }
  }
}

/*
* writes a line word by word to a given file
*
* @param line to write
* @param fd file descriptor to write to
*/
void *
writewords(char *line, int fd){
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
* @param count max num of lines or words
* @param lines array of lines or words
* @param cflag boolean for the -c flag
*/
void print_uniq(int count, char *lines[], bool cflag) {
  int curr = 0; // keeps track of which line we're at
  int instances = 1; // keeps track of how many instances

  while(curr < count) { // while we still have stuff to read
    // skip null terms and line ends
    if ((strcmp(lines[curr], "\0") == 0) || (strcmp(lines[curr], "\n") == 0)
            || (strcmp(lines[curr], "\t") == 0)) {
       curr++;
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
void lines(int fd, bool cflag) {
  uint sz = 20; // getline will resize if necessary
  int count = 0; // num of lines
  char *lines[1024];

  while (true) {
    char *line = malloc(sz);
    if (getline(&line, &sz, fd) <= 0) {
      break;
    }
    char *temp = malloc(sz);
    memcpy(temp, line, sz);
    free(line); // free old memory
    lines[count] = temp;
    line = temp;
    count++;
  } 
  
  bubble_sort(lines, count); // sort all the lines
  print_uniq(count, lines, cflag); // print uniq instances
  for (int i = 0; i < count; i++) {
  	free(lines[i]);
  }
}

/*
* creates a char * of all the words from a given file and
* prints out all the unique words
*
* @param fd file descriptor of file to read
* @param cflag boolean for the -c flag
*/
void words(int fd, bool cflag){
  uint sz = 20; // getline will resize if necessary
  int count = 0; // num of lines
  // file we are writing the words to
  int write = open("/temp.txt", O_CREATE | O_WRONLY);
  if (write < 0) {
    printf("error opening file: temp.txt\n");
    return;
  }

  while (true) {
    char *line = malloc(sz);
    if (getline(&line, &sz, fd) <= 0) {
      break;
    }
    char *temp = malloc(sz);
    memcpy(temp, line, sz);
    free(line); // free old memory

    writewords(temp, write);
    line = temp;
    count++;
  }
  close(write);
  // have to close & reopen file because xv6 hates us

  // file to now read the words from
  int read = open("/temp.txt", O_RDONLY);
  if (read < 0) {
    printf("error opening file: temp.txt\n");
    return;
  }

	int wordcount = 0; // num words
  char *words[1024];
  uint newsize = 10;

  while (true) {
    char *word = malloc(newsize);
    if(getline(&word, &newsize, read) <= 0) {
      break;
    }
    words[wordcount] = word;
    wordcount++;
  }

  close(read);
  bubble_sort(words, wordcount); // sort all the words
  print_uniq(wordcount, words, cflag); // print unique words
  // for (int i = 0; i < wordcount; i++) {
  //  	free(words[i]);
  // }
}

/*
* creates a char * of all the lines from a given file and
* prints out all the unique lines
*
* @param argv array of args to check for flags
* @param fd file descriptor of file to read
*/
void
uniq(char *argv[], int fd) {
  int j = 0; 

  //FINDING PROPER FLAGS
  while (argv[j] != NULL) {
    if (!strcmp(argv[j], "-c")) {
      lines(fd, true);
      return;
    }

    else if (!strcmp(argv[j], "-wc")) {
      words(fd, true);
      return;
    }
  
    else if (!strcmp(argv[j], "-w")) {
      words(fd, false);
      return;
    }
    j++;
  }
  
  lines(fd, false);
}

int main(int argc, char *argv[]){

	if (argc < 2) {
		printf("usage: uniq filename [-w] [-c] [-wc]\n");
		return -1;
	}

  int fd = open(argv[1], O_RDONLY);
  if (fd < 1) {
  	printf("err opening file\n");
  	return -1;
  }
  uniq(argv, fd);
  return 0;

}

