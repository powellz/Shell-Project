
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "TreeDictionary.h"

void
printUsage() {
	printf("Usage: TreeTest test1|test2|test3...test9\n");
}

const char *names[] = { "pineapple","kiwi", "orange", "grapes",
			"strawberry", "apple", "orange", "watermelon", "lime", "olives", "mango"};

int
main(int argc, char ** argv)
{
	
	if (argc < 2) {
		printUsage();
		exit(1);
	}

	char * testNum = argv[1];

	if (!strcmp(testNum, "test1")) {
		TreeDictionary tree;
		tree.addRecord("banana",1);
		tree.addRecord("apple",1);
		tree.addRecord("kiwi",1);
		tree.print();
	}
	else if (!strcmp(testNum, "test2")) {
		TreeDictionary tree;
		int n = sizeof(names)/sizeof(const char*);
		for (int i = 0; i < n; i++) {
		  tree.addRecord(names[i],i);
		}
		tree.print();
	}
	else if (!strcmp(testNum, "test3")) {
		TreeDictionary tree;
		int n = sizeof(names)/sizeof(const char*);
		for (int i = 0; i < n; i++) {
		  tree.addRecord(names[i],i);
		}
		tree.print();
		int data = 0;
		bool result = tree.findRecord("kiwi", data);
		printf("kiwi data=%d result=%s\n", data, result?"true":"false");
		result = tree.findRecord("peach", data);
		printf("peach data=%d result=%s\n", data, result?"true":"false");
	}
	else if (!strcmp(testNum, "test4")) {
		TreeDictionary tree;
		int n = sizeof(names)/sizeof(const char*);
		for (int i = 0; i < n; i++) {
		  tree.addRecord(names[i],i);
		}
		tree.print();
		printf("numberOfEntries=%d\n", tree.numberOfEntries());
	}
	else if (!strcmp(testNum, "test5")) {
		TreeDictionary tree;
		int n = sizeof(names)/sizeof(const char*);
		for (int i = 0; i < n; i++) {
		  tree.addRecord(names[i],i);
		}
		tree.printIndented();		
	}
	else if (!strcmp(testNum, "test6")) {
		TreeDictionary tree;
		int n = sizeof(names)/sizeof(const char*);
		tree.printIndented();
		printf("maxDepth Before=%d\n", tree.maxDepth());
		for (int i = 0; i < n; i++) {
		  tree.addRecord(names[i],i);
		}
		tree.printIndented();
		printf("maxDepth After=%d\n", tree.maxDepth());
	}
	else if (!strcmp(testNum, "test7")) {
		TreeDictionary tree;
		int n = sizeof(names)/sizeof(const char*);
		for (int i = 0; i < n; i++) {
		  tree.addRecord(names[i],i);
		}
		tree.printIndented();
		printf("minKey=%s\n", tree.minimumKey());
	}
	else if (!strcmp(testNum, "test8")) {
		TreeDictionary tree;
		int n = sizeof(names)/sizeof(const char*);
		for (int i = 0; i < n; i++) {
		  tree.addRecord(names[i],i);
		}
		tree.printIndented();
		printf("maxKey=%s\n", tree.maximumKey());
	}
	else if (!strcmp(testNum, "test9")) {
		TreeDictionary tree;
		int n = sizeof(names)/sizeof(const char*);
		for (int i = 0; i < n; i++) {
		  tree.addRecord(names[i],i);
		}
		tree.print();
		int nentries;
		const char ** keys = tree.keys(nentries);
		for (int i = 0; i < nentries; i++) {
		  printf("key:%s\n", keys[i]);
		}
	}
	else {
		printf("Unknonwn test\n");
	}
}
