#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
//#include "user/sortfuncs.h"
#include <stddef.h>

int
main(int argc, char *argv[])
{
  if (argc != 2) {
  	printf("Usage: sort <filename>\n");
  	return -1;
  }

  char *file_name = argv[1];

  int fd = open(file_name, O_RDONLY);
  if (fd < 0) {
  	printf("sort: cannot open %s\n", file_name);
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


/*
* Add sort functions or extend modularity.
*/
int
sort(int fd, int num_flags, char *flags[])
{
	return 0;
}
