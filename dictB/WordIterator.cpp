
#include <stdio.h>
#include <stdlib.h>
#include "WordIterator.h"

WordIterator::WordIterator()
{
}

bool
WordIterator::readFile(const char * fileName)
{
	_fd = fopen(fileName, "r");
	if (_fd == NULL)	
		return false;
	return true;
}

const char * 
WordIterator::next()
{
	if (fscanf(_fd, "%s", word) == 1)
		return word;
	return NULL;
}

WordIterator::~WordIterator()
{
	free(_fd);
}

