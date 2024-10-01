## uniq Command

### Pupose
Reports or delets repeated lines in a file.

### Syntax
`uniq [-c | -t | -f]`

### Description
The `uniq` command removes consecutive duplicate lines from a file. It reads input
from either standard input or a specified file and compares adjacent lines to eliminate
repeated occurrences. Because only adjacent duplicate lines are removed it is sypically
used in conjusction with the `sort` command to ensure all duplicates are adjacent. The
unique lines are then written to standard output.

### Flags
- **-c**: prints the count of the unique lines
- **-t**: output the total number of unique lines or words at the end
- **-f**: allows the user to ignore case (case-insensitive comparison for uniqueness)

### Example
To remove repeated adjacent lines, enter:
`uniq README.md`

To remove nonadjacent repeated lines, enter:
`sort README.md | uniq` 

To remove nonadjacent repeated words, enter:
`split README.md | sort | uniq`

### Bugs
- The **-f** currently does not produce intended output on nonadjacent words.
- If the file being processed includes any empty lines the `uniq` command will break.
