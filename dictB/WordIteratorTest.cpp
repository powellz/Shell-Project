
#include "WordIterator.h"
#include <stdlib.h>
#include <string.h>

void
printUsage() {
	printf("Usage: TreeTest test1|test2|test3\n");
}

int
main(int argc, char ** argv)
{
  	if (argc < 2) {
		printUsage();
		exit(1);
	}

	char * testNum = argv[1];

	WordIterator witerator;
	bool exists;

	if (!strcmp(testNum, "test1")) {
	  exists = witerator.readFile("hamlet-short.txt");
	}
	else if (!strcmp(testNum, "test2")) {
	  exists = witerator.readFile("hamlet-long.txt");
	}
	else if (!strcmp(testNum, "test3")) {
	  exists = witerator.readFile("cosmos.txt");
	}

	if (!exists) {
	  printf("File does not exist\n");
	  exit(1);
	}

	const char * word;
	int i = 0;
	while ( word = witerator.next()) {
	//while (i < 38) {
	  printf("%d:%s\n", i,word);
	  i++;
	}
}

