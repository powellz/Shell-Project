
goal: TreeDictionaryTest WordIteratorTest WordCounter

TreeDictionaryTest: TreeDictionaryTest.cpp TreeDictionary.cpp 
	g++ -g -o TreeDictionaryTest TreeDictionaryTest.cpp TreeDictionary.cpp 

WordIteratorTest: WordIteratorTest.cpp  WordIterator.cpp
	g++ -g -o WordIteratorTest WordIteratorTest.cpp WordIterator.cpp

WordCounter: WordCounter.cpp WordIterator.cpp TreeDictionary.cpp 
	g++ -g -o WordCounter WordCounter.cpp WordIterator.cpp TreeDictionary.cpp

clean:
	rm -rf TreeDictionaryTest WordIteratorTest WordCounter *.out
